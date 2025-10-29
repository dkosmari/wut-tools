#include "utils.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <excmd.h>
#include <fmt/base.h>
#include <fmt/ostream.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <locale>
#include <string>
#include <vector>
#include <zlib.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

using std::cout;
using std::cerr;
using std::endl;

/*
.extern __preinit_user

.section .fexports, "", @0x80000001
.align 4

.long 1
.long 0x13371337

.long __preinit_user
.long 0x10

.string "__preinit_user"
.byte 0
 */

enum class ReadMode
{
   INVALID,
   TEXT,
   DATA,
   NAME
};

void
writeExports(std::ofstream &out,
             bool isData,
             const std::vector<std::string> &exports)
{
   // Calculate signature
   uint32_t signature = crc32(0, Z_NULL, 0);
   for (const auto &name : exports) {
      signature = crc32(signature, reinterpret_cast<const Bytef *>(name.data()), name.size() + 1);
   }

   // Write out .extern to declare the symbols
   for (const auto &name : exports) {
      out << ".extern " << name << '\n';
   }
   out << '\n';

   // Write out header
   if (isData) {
      out << ".section .dexports, \"a\", @0x80000001\n";
   } else {
      out << ".section .fexports, \"ax\", @0x80000001\n";
   }

   out << ".align 4\n\n";

   out << ".long " << exports.size() << '\n';
   out << ".long 0x" << std::hex << signature << "\n\n";

   // Write out each export
   auto nameOffset = 8 + 8 * exports.size();
   for (const auto &name : exports) {
      out << ".long " << name << '\n';
      out << ".long 0x" << std::hex << nameOffset << '\n';
      nameOffset += name.size() + 1;
   }
   out << '\n';

   // Write out the strings
   for (const auto &name : exports) {
      out << ".string \"" << name << "\"\n";
      nameOffset += name.size() + 1;
   }
   out << '\n';
}

static void
show_help(std::ostream& out,
          const excmd::parser& parser,
          const std::string& exec_name)
{
   fmt::print(out, "{} [options] <exports.def> <output.S>\n", exec_name);
   fmt::print(out, "{}\n", parser.format_help(exec_name));
   fmt::print(out, "Report bugs to {}\n", PACKAGE_BUGREPORT);
}

int main(int argc, char **argv)
{
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
                     description { "Show version" });

      parser.default_command()
         .add_argument("<exports.def>",
                       description { "Path to input exports def file" },
                       value<std::string> {})
         .add_argument("<output.S>",
                       description { "Path to output assembly file" },
                       value<std::string> {});

      options = parser.parse(argc, argv);
   }
   catch (std::exception& ex) {
      cerr << "Error parsing options: " << ex.what() << endl;
      return -1;
   }

   if (options.has("help")) {
      show_help(cout, parser, argv[0]);
      return 0;
   }

   if (options.has("version")) {
      fmt::print("{} ({}) {}\n", argv[0], PACKAGE_NAME, PACKAGE_VERSION);
      return 0;
   }

   if (!options.has("<exports.def>") || !options.has("<output.S>")) {
      cerr << "Missing mandatory arguments: <exports.def> <output.S>.\n";
      show_help(cerr, parser, argv[0]);
      return -1;
   }

   {
      auto src = options.get<std::string>("<exports.def>");
      std::ifstream in{src};

      if (!in.is_open()) {
         std::cout << "Could not open file " << src << " for reading" << std::endl;
         return -1;
      }

      std::string line;
      while (getline(in, line)) {
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
            } else if (line.substr(1) == "DATA") {
               readMode = ReadMode::DATA;
            } else if (line.substr(1, 4) == "NAME") {
               readMode = ReadMode::NAME;
            } else {
               cerr << "Unexpected section type" << endl;
               return -1;
            }
            continue;
         }

         if (readMode == ReadMode::TEXT) {
            funcExports.push_back(line);
         } else if (readMode == ReadMode::DATA) {
            dataExports.push_back(line);
         } else if (readMode == ReadMode::NAME) {
            // We can ignore name in rplexportgen
         } else {
            cerr << "Unexpected section data" << endl;
            return -1;
         }
      }
   }

   // Exports must be in alphabetical order because loader.elf uses binary search
   std::sort(funcExports.begin(), funcExports.end());
   std::sort(dataExports.begin(), dataExports.end());

   {
      auto dst = options.get<std::string>("<output.S>");
      std::ofstream out{dst};

      if (!out.is_open()) {
         cerr << "Could not open file " << dst << " for writing" << endl;
         return -1;
      }

      if (funcExports.size() > 0) {
         writeExports(out, false, funcExports);
      }

      if (dataExports.size() > 0) {
         writeExports(out, true, dataExports);
      }
   }
}
