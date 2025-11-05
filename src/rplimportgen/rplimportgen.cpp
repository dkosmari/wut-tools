#include "utils.h"
#include "rplwrap.h"

#include <cstdint>
#include <cstddef>
#include <excmd.h>
#include <fmt/base.h>
#include <fmt/ostream.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

using std::cerr;
using std::cout;

enum class ReadMode
{
   INVALID,
   TEXT,
   TEXT_WRAP,
   DATA,
   DATA_WRAP,
};

static void
writeExports(std::ostream &out,
             const std::string &moduleName,
             bool isData,
             const std::vector<std::string> &exports)
{
   if (isData) {
      fmt::println(out, ".section .dimport_{}, \"a\", @0x80000002", moduleName);
   } else {
      fmt::println(out, ".section .fimport_{}, \"ax\", @0x80000002", moduleName);
   }

   fmt::println(out, ".align 4\n");

   // Usually the symbol count, but isn't checked on hardware.
   // Spoofed to allow ld to garbage-collect later.
   fmt::println(out, ".long 1");
   // Supposed to be a crc32 of the imports. Again, not actually checked.
   fmt::println(out, ".long 0x00000000\n");

   fmt::println(out, ".ascii \"{}\"", moduleName);
   // Pad with zeros to make length a multiple of 8.
   std::size_t next_multiple = (moduleName.size() + 1ull + 8ull) & ~7ull;
   std::size_t padding = next_multiple - moduleName.size();
   fmt::println(out, ".skip {}", padding);
   fmt::println(out, "");

   const char *type = isData ? "@object" : "@function";

   for (auto& name : exports) {
      // Basically do -ffunction-sections
      if (isData) {
         fmt::println(out, ".section .dimport_{}.{}, \"a\", @0x80000002", moduleName, name);
      } else {
         fmt::println(out, ".section .fimport_{}.{}, \"ax\", @0x80000002", moduleName, name);
      }
      fmt::println(out, ".global {}", name);
      fmt::println(out, ".type {}, {}", name, type);
      fmt::println(out, "{}:", name);
      fmt::println(out, ".long 0x0");
      fmt::println(out, ".long 0x0\n");
   }
}

static void
writeLinkerScript(std::ostream &out,
                  const std::string &name)
{
   fmt::print(out,
              "SECTIONS\n"
              "{{\n"
              "   .fimport_{0} ALIGN(16) : {{\n"
              "      KEEP ( *(.fimport_{0}) )\n"
              "      *(.fimport_{0}.*)\n"
              "   }} > loadmem\n"
              "   .dimport_{0} ALIGN(16) : {{\n"
              "      KEEP ( *(.dimport_{0}) )\n"
              "      *(.dimport_{0}.*)\n"
              "   }} > loadmem\n"
              "}}\n",
              name);
}

static void
show_help(std::ostream& out,
          const excmd::parser& parser,
          const std::string& exec_name)
{
   fmt::println(out, "{} [options] <exports.def> <output.S> [<output.ld>]", exec_name);
   fmt::println(out, "{}", parser.format_help(exec_name));
   fmt::println(out, "Report bugs to {}", PACKAGE_BUGREPORT);
}

int
main(int argc, char **argv)
{
   std::string moduleName;
   std::vector<std::string> funcExports, dataExports;
   ReadMode readMode = ReadMode::INVALID;
   excmd::parser parser;
   excmd::option_state options;

   try {
      using excmd::description;
      using excmd::value;
      parser.global_options()
         .add_option("H,help",
                     description { "Show help" })
         .add_option("v,version",
                     description { "Show version" })
         ;

      parser.default_command()
         .add_argument("<exports.def>",
                       description { "Path to input exports def file" },
                       value<std::string> {})
         .add_argument("<output.S>",
                       description { "Path to output assembly file" },
                       value<std::string> {})
         .add_argument("<output.ld>",
                       description { "Path to output linker script" },
                       excmd::optional {},
                       value<std::string> {})
         ;

      options = parser.parse(argc, argv);

   }
   catch (std::exception& ex) {
      fmt::println(cerr, "Error parsing options: {}", ex.what());
      return -1;
   }

   if (options.has("help")) {
      show_help(cout, parser, argv[0]);
      return 0;
   }

   if (options.has("version")) {
      fmt::println("{} ({}) {}", argv[0], PACKAGE_NAME, PACKAGE_VERSION);
      return 0;
   }

   if (!options.has("<exports.def>") || !options.has("<output.S>")) {
      fmt::println(cerr, "Missing mandatory arguments: <exports.def> <output.S>");
      show_help(cerr, parser, argv[0]);
      return -1;
   }

   {
      auto exports_def = options.get<std::string>("<exports.def>");
      std::ifstream in{exports_def};

      if (!in.is_open()) {
         fmt::println(cerr, "Could not open file \"{}\" for reading.", exports_def);
         return -1;
      }

      std::string line;
      while (std::getline(in, line)) {
         // Trim comments
         std::size_t commentOffset = line.find("//");
         if (commentOffset != std::string::npos) {
            line = line.substr(0, commentOffset);
         }

         // Trim whitespace
         line = trim(line);

         // Skip blank lines
         if (line.length() == 0) {
            continue;
         }

         // Look for section headers
         if (line[0] == ':') {
            if (line.substr(1) == "TEXT") {
               readMode = ReadMode::TEXT;
            } else if (line.substr(1) == "TEXT_WRAP") {
               readMode = ReadMode::TEXT_WRAP;
            } else if (line.substr(1) == "DATA") {
               readMode = ReadMode::DATA;
            } else if (line.substr(1) == "DATA_WRAP") {
               readMode = ReadMode::DATA_WRAP;
            } else if (line.substr(1, 4) == "NAME") {
               moduleName = line.substr(6);
            } else {
               fmt::println(cerr, "Unexpected section type: {}", line.substr(1));
               return -1;
            }
            continue;
         }

         if (readMode == ReadMode::TEXT) {
            funcExports.push_back(line);
         } else if (readMode == ReadMode::TEXT_WRAP) {
            funcExports.push_back(std::string(RPLWRAP_PREFIX) + line);
         } else if (readMode == ReadMode::DATA) {
            dataExports.push_back(line);
         } else if (readMode == ReadMode::DATA_WRAP) {
            dataExports.push_back(std::string(RPLWRAP_PREFIX) + line);
         } else {
            fmt::println(cerr, "Unexpected section data.");
            return -1;
         }
      }
   }

   {
      auto output_S = options.get<std::string>("<output.S>");
      std::ofstream out{output_S};

      if (!out.is_open()) {
         fmt::println(cerr, "Could not open file \"{}\" for writing.", output_S);
         return -1;
      }

      if (funcExports.size() > 0) {
         writeExports(out, moduleName, false, funcExports);
      }

      if (dataExports.size() > 0) {
         writeExports(out, moduleName, true, dataExports);
      }
   }

   if (options.has("<output.ld>")) {
      auto output_ld = options.get<std::string>("<output.ld>");
      std::ofstream out{output_ld};

      if (!out.is_open()) {
         fmt::println(cerr, "Could not open file \"{}\" for writing.", output_ld);
         return -1;
      }

      writeLinkerScript(out, moduleName);
   }
}
