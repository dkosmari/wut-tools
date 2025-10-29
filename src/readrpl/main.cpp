#include "elf.h"
#include "generate_exports_def.h"
#include "print.h"
#include "verify.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <excmd.h>
#include <fmt/base.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <zlib.h>

using std::cerr;
using std::cout;
using std::endl;

enum ErrorCodes {
   ERROR_BAD_ARGUMENTS = 1,
   ERROR_OPEN_INPUT    = 2,
   ERROR_BAD_INPUT     = 3,
   ERROR_OPEN_OUTPUT   = 4,
};

static std::string
getFileBasename(std::string path)
{
   auto pos = path.find_last_of("\\/");
   if (pos != std::string::npos) {
      path.erase(0, pos + 1);
   }

   pos = path.rfind('.');
   if (pos != std::string::npos) {
      path.erase(pos);
   }

   return path;
}

uint32_t
getSectionIndex(const Rpl &rpl,
                const Section &section)
{
   return static_cast<uint32_t>(&section - &rpl.sections[0]);
}

bool
readSection(std::ifstream &fh,
            Section &section,
            size_t i)
{
   // Read section header
   fh.read(reinterpret_cast<char*>(&section.header), sizeof(elf::SectionHeader));

   if (section.header.type == elf::SHT_NOBITS || !section.header.size) {
      return true;
   }

   // Read section data
   if (section.header.flags & elf::SHF_DEFLATED) {
      auto stream = z_stream {};
      auto ret = Z_OK;

      // Read the original size
      uint32_t size = 0;
      fh.seekg(section.header.offset.value());
      fh.read(reinterpret_cast<char *>(&size), sizeof(uint32_t));
      size = byte_swap(size);
      section.data.resize(size);

      // Inflate
      memset(&stream, 0, sizeof(stream));
      stream.zalloc = Z_NULL;
      stream.zfree = Z_NULL;
      stream.opaque = Z_NULL;

      ret = inflateInit(&stream);

      if (ret != Z_OK) {
         fmt::print(cerr, "Couldn't decompress .rpx section because inflateInit returned {}\n", ret);
         section.data.clear();
         return false;
      } else {
         std::vector<char> temp;
         temp.resize(section.header.size-sizeof(uint32_t));
         fh.read(temp.data(), temp.size());

         stream.avail_in = section.header.size;
         stream.next_in = reinterpret_cast<Bytef *>(temp.data());
         stream.avail_out = static_cast<uInt>(section.data.size());
         stream.next_out = reinterpret_cast<Bytef *>(section.data.data());

         ret = inflate(&stream, Z_FINISH);

         if (ret != Z_OK && ret != Z_STREAM_END) {
            fmt::print(cerr, "Couldn't decompress .rpx section because inflate returned {}\n", ret);
            section.data.clear();
            return false;
         }

         inflateEnd(&stream);
      }
   } else {
      section.data.resize(section.header.size);
      fh.seekg(section.header.offset.value());
      fh.read(section.data.data(), section.header.size);
   }

   return true;
}

static void
show_help(std::ostream& out,
          const excmd::parser& parser,
          const std::string& exec_name)
{
   fmt::print(out, "{} [options] path\n", exec_name);
   fmt::print(out, "{}\n", parser.format_help(exec_name));
   fmt::print(out, "Report bugs to {}\n", PACKAGE_BUGREPORT);
}

int main(int argc, char **argv)
{
   excmd::parser parser;
   excmd::option_state options;
   using excmd::description;
   using excmd::value;

   try {
      parser.global_options()
         .add_option("H,help",
                     description { "Show help" })
         .add_option("v,version",
                     description { "Show version" })
         .add_option("a,all",
                     description { "Equivalent to: -h -S -s -r -i -x -c -f" })
         .add_option("h,file-header",
                     description { "Display the ELF file header" })
         .add_option("S,sections",
                     description { "Display the sections' header" })
         .add_option("s,symbols",
                     description { "Display the symbol table" })
         .add_option("r,relocs",
                     description { "Display the relocations" })
         .add_option("i,imports",
                     description { "Display the RPL imports" })
         .add_option("x,exports",
                     description { "Display the RPL exports" })
         .add_option("c,crc",
                     description { "Display the RPL crc" })
         .add_option("f,file-info",
                     description { "Display the RPL file info" })
         .add_option("exports-def",
                     description { "Generate exports.def for wut library linking" },
                     value<std::string> {});

      parser.default_command()
         .add_argument("path",
                       description { "Path to RPL file" },
                       value<std::string> {});

      options = parser.parse(argc, argv);
   } catch (std::exception& ex) {
      cerr << "Error parsing options: " << ex.what() << endl;
      return ERROR_BAD_ARGUMENTS;
   }

   if (options.has("help")) {
      show_help(cout, parser, argv[0]);
      return 0;
   }

   if (options.has("version")) {
      fmt::print("{} ({}) {}\n", argv[0], PACKAGE_NAME, PACKAGE_VERSION);
      return 0;
   }

   if (options.empty()) {
      cerr << "No option provided.\n";
      show_help(cerr, parser, argv[0]);
      return ERROR_BAD_ARGUMENTS;
   }

   if (!options.has("path")) {
      cerr << "Error: path argument is mandatory.\n";
      show_help(cerr, parser, argv[0]);
      return ERROR_BAD_ARGUMENTS;
   }

   auto all = options.has("all");

   auto dumpElfHeader = all || options.has("file-header");
   auto dumpSectionSummary = all || options.has("sections");
   auto dumpSectionRela = all || options.has("relocs");
   auto dumpSectionSymtab = all || options.has("symbols");
   auto dumpSectionRplExports = all || options.has("exports");
   auto dumpSectionRplImports = all || options.has("imports");
   auto dumpSectionRplCrcs = all || options.has("crc");
   auto dumpSectionRplFileinfo = all || options.has("file-info");
   auto path = options.get<std::string>("path");

   // If no options are set (other than "path"), let's default to a summary
   if (options.set_options.size() == 1) {
      dumpElfHeader = true;
      dumpSectionSummary = true;
      dumpSectionRplFileinfo = true;
   }

   // Read file
   std::ifstream fh { path, std::ifstream::binary };
   if (!fh.is_open()) {
       fmt::print(cerr, "Could not open {} for reading\n", path);
      return ERROR_OPEN_INPUT;
   }

   Rpl rpl;
   fh.read(reinterpret_cast<char*>(&rpl.header), sizeof(elf::Header));

   if (rpl.header.magic != elf::HeaderMagic) {
      cerr << "Invalid ELF magic header\n";
      return ERROR_BAD_INPUT;
   }

   // Read sections
   for (auto i = 0u; i < rpl.header.shnum; ++i) {
      Section section;
      fh.seekg(rpl.header.shoff + rpl.header.shentsize * i);

      if (!readSection(fh, section, i)) {
         fmt::print("Error reading section {}", i);
         return ERROR_BAD_INPUT;
      }

      rpl.sections.push_back(section);
   }

   // Set section names
   auto shStrTab = reinterpret_cast<const char *>(rpl.sections[rpl.header.shstrndx].data.data());
   for (auto &section : rpl.sections) {
      section.name = shStrTab + section.header.name;
   }

   // Verify rpl format
   verifyFile(rpl);
   verifyCrcs(rpl);
   verifyFileBounds(rpl);
   verifyRelocationTypes(rpl);
   verifySectionAlignment(rpl);
   verifySectionOrder(rpl);

   // Format shit
   if (dumpElfHeader) {
      printHeader(rpl);
   }

   if (dumpSectionSummary) {
      printSectionSummary(rpl);
   }

   // Print section data
   for (auto i = 0u; i < rpl.sections.size(); ++i) {
      auto &section = rpl.sections[i];
      auto printSectionHeader = [&](){
         fmt::print(
            "Section {}: {}, {}, {} bytes\n",
            i, formatSHT(section.header.type), section.name, section.data.size());
      };

      switch (section.header.type) {
      case elf::SHT_NULL:
      case elf::SHT_NOBITS:
         // Print nothing
         break;
      case elf::SHT_RELA:
         if (!dumpSectionRela) {
            continue;
         }

         printSectionHeader();
         printRela(rpl, section);
         break;
      case elf::SHT_SYMTAB:
         if (!dumpSectionSymtab) {
            continue;
         }

         printSectionHeader();
         printSymTab(rpl, section);
         break;
      case elf::SHT_STRTAB:
         break;
      case elf::SHT_PROGBITS:
         break;
      case elf::SHT_RPL_EXPORTS:
         if (!dumpSectionRplExports) {
            continue;
         }

         printSectionHeader();
         printRplExports(rpl, section);
         break;
      case elf::SHT_RPL_IMPORTS:
         if (!dumpSectionRplImports) {
            continue;
         }

         printSectionHeader();
         printRplImports(rpl, section);
         break;
      case elf::SHT_RPL_CRCS:
         if (!dumpSectionRplCrcs) {
            continue;
         }

         printSectionHeader();
         printRplCrcs(rpl, section);
         break;
      case elf::SHT_RPL_FILEINFO:
         if (!dumpSectionRplFileinfo) {
            continue;
         }

         printSectionHeader();
         printFileInfo(rpl, section);
         break;
      }
   }

   if (options.has("exports-def")) {
      auto output = options.get<std::string>("exports-def");
      if (!generateExportsDef(rpl, getFileBasename(path), output)) {
         return ERROR_OPEN_OUTPUT;
      }
   }
}
