#include "print.h"
#include <fmt/base.h>
#include <iostream>
#include <string>

using std::cout;

static std::string
formatET(uint32_t type)
{
   switch (type) {
   case elf::ET_NONE:
      return "ET_NONE";
   case elf::ET_REL:
      return "ET_REL";
   case elf::ET_EXEC:
      return "ET_EXEC";
   case elf::ET_DYN:
      return "ET_DYN";
   case elf::ET_CORE:
      return "ET_CORE";
   case elf::ET_CAFE_RPL:
      return "ET_CAFE_RPL";
   default:
      return fmt::format("{}", type);
   }
}

static std::string
formatEM(uint32_t machine)
{
   switch (machine) {
   case elf::EM_PPC:
      return "EM_PPC";
   default:
      return fmt::format("{}", machine);
   }
}

static std::string
formatEABI(uint32_t eabi)
{
   switch (eabi) {
   case elf::EABI_CAFE:
      return "EABI_CAFE";
   default:
      return fmt::format("{}", eabi);
   }
}

static std::string
formatSHF(uint32_t flags)
{
   std::string result = "";

   if (flags & elf::SHF_WRITE) {
      result += "W";
   }

   if (flags & elf::SHF_ALLOC) {
      result += "A";
   }

   if (flags & elf::SHF_EXECINSTR) {
      result += "X";
   }

   if (flags & elf::SHF_DEFLATED) {
      result += "Z";
   }

   return result;
}

std::string
formatSHT(uint32_t type)
{
   switch (type) {
   case elf::SHT_NULL:
      return "SHT_NULL";
   case elf::SHT_PROGBITS:
      return "SHT_PROGBITS";
   case elf::SHT_SYMTAB:
      return "SHT_SYMTAB";
   case elf::SHT_STRTAB:
      return "SHT_STRTAB";
   case elf::SHT_RELA:
      return "SHT_RELA";
   case elf::SHT_HASH:
      return "SHT_HASH";
   case elf::SHT_DYNAMIC:
      return "SHT_DYNAMIC";
   case elf::SHT_NOTE:
      return "SHT_NOTE";
   case elf::SHT_NOBITS:
      return "SHT_NOBITS";
   case elf::SHT_REL:
      return "SHT_REL";
   case elf::SHT_SHLIB:
      return "SHT_SHLIB";
   case elf::SHT_DYNSYM:
      return "SHT_DYNSYM";
   case elf::SHT_INIT_ARRAY:
      return "SHT_INIT_ARRAY";
   case elf::SHT_FINI_ARRAY:
      return "SHT_FINI_ARRAY";
   case elf::SHT_PREINIT_ARRAY:
      return "SHT_PREINIT_ARRAY";
   case elf::SHT_GROUP:
      return "SHT_GROUP";
   case elf::SHT_SYMTAB_SHNDX:
      return "SHT_SYMTAB_SHNDX";
   case elf::SHT_LOPROC:
      return "SHT_LOPROC";
   case elf::SHT_HIPROC:
      return "SHT_HIPROC";
   case elf::SHT_LOUSER:
      return "SHT_LOUSER";
   case elf::SHT_RPL_EXPORTS:
      return "SHT_RPL_EXPORTS";
   case elf::SHT_RPL_IMPORTS:
      return "SHT_RPL_IMPORTS";
   case elf::SHT_RPL_CRCS:
      return "SHT_RPL_CRCS";
   case elf::SHT_RPL_FILEINFO:
      return "SHT_RPL_FILEINFO";
   case elf::SHT_HIUSER:
      return "SHT_HIUSER";
   default:
      return fmt::format("{}", type);
   }
}

static std::string
formatRelType(uint32_t type)
{
   switch (type) {
   case elf::R_PPC_NONE:
      return "NONE";
   case elf::R_PPC_ADDR32:
      return "ADDR32";
   case elf::R_PPC_ADDR16_LO:
      return "ADDR16_LO";
   case elf::R_PPC_ADDR16_HI:
      return "ADDR16_HI";
   case elf::R_PPC_ADDR16_HA:
      return "ADDR16_HA";
   case elf::R_PPC_REL24:
      return "REL24";
   case elf::R_PPC_REL14:
      return "REL14";
   case elf::R_PPC_DTPMOD32:
      return "DTPMOD32";
   case elf::R_PPC_DTPREL32:
      return "DTPREL32";
   case elf::R_PPC_EMB_SDA21:
      return "EMB_SDA21";
   case elf::R_PPC_EMB_RELSDA:
      return "EMB_RELSDA";
   case elf::R_PPC_DIAB_SDA21_LO:
      return "DIAB_SDA21_LO";
   case elf::R_PPC_DIAB_SDA21_HI:
      return "DIAB_SDA21_HI";
   case elf::R_PPC_DIAB_SDA21_HA:
      return "DIAB_SDA21_HA";
   case elf::R_PPC_DIAB_RELSDA_LO:
      return "DIAB_RELSDA_LO";
   case elf::R_PPC_DIAB_RELSDA_HI:
      return "DIAB_RELSDA_HI";
   case elf::R_PPC_DIAB_RELSDA_HA:
      return "DIAB_RELSDA_HA";
   case elf::R_PPC_GHS_REL16_HA:
      return "GHS_REL16_HA";
   case elf::R_PPC_GHS_REL16_HI:
      return "GHS_REL16_HI";
   case elf::R_PPC_GHS_REL16_LO:
      return "GHS_REL16_LO";
   default:
      return fmt::format("{}", type);
   }
}

static std::string
formatSymType(uint32_t type)
{
   switch (type) {
   case elf::STT_NOTYPE:
      return "NOTYPE";
   case elf::STT_OBJECT:
      return "OBJECT";
   case elf::STT_FUNC:
      return "FUNC";
   case elf::STT_SECTION:
      return "SECTION";
   case elf::STT_FILE:
      return "FILE";
   case elf::STT_COMMON:
      return "COMMON";
   case elf::STT_TLS:
      return "TLS";
   case elf::STT_LOOS:
      return "LOOS";
   case elf::STT_HIOS:
      return "HIOS";
   case elf::STT_GNU_IFUNC:
      return "GNU_IFUNC";
   default:
      return fmt::format("{}", type);
   }
}

static std::string
formatSymBinding(uint32_t type)
{
   switch (type) {
   case elf::STB_LOCAL:
      return "LOCAL";
   case elf::STB_GLOBAL:
      return "GLOBAL";
   case elf::STB_WEAK:
      return "WEAK";
   case elf::STB_GNU_UNIQUE:
      return "UNIQUE";
   default:
      return fmt::format("{}", type);
   }
}

static std::string
formatSymShndx(uint32_t type)
{
   switch (type) {
   case elf::SHN_UNDEF:
      return "UND";
   case elf::SHN_ABS:
      return "ABS";
   case elf::SHN_COMMON:
      return "CMN";
   case elf::SHN_XINDEX:
      return "UND";
   default:
      return fmt::format("{}", type);
   }
}

void
printHeader(const Rpl &rpl)
{
   const auto &header = rpl.header;
   fmt::println(cout, "ElfHeader");
   fmt::println(cout, "  {:<20} = 0x{:08X}",    "magic",      header.magic.value());
   fmt::println(cout, "  {:<20} = {}",          "fileClass",  unsigned{header.fileClass});
   fmt::println(cout, "  {:<20} = {}",          "encoding",   unsigned{header.encoding});
   fmt::println(cout, "  {:<20} = {}",          "elfVersion", unsigned{header.elfVersion});
   fmt::println(cout, "  {:<20} = {} 0x{:04x}", "abi",        formatEABI(header.abi), header.abi.value());
   fmt::println(cout, "  {:<20} = {} 0x{:04X}", "type",       formatET(header.type), header.type.value());
   fmt::println(cout, "  {:<20} = {} {}",       "machine",    formatEM(header.machine), header.machine);
   fmt::println(cout, "  {:<20} = 0x{:X}",      "version",    header.version.value());
   fmt::println(cout, "  {:<20} = 0x{:08X}",    "entry",      header.entry.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",      "phoff",      header.phoff.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",      "shoff",      header.shoff.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",      "flags",      header.flags.value());
   fmt::println(cout, "  {:<20} = {}",          "ehsize",     header.ehsize);
   fmt::println(cout, "  {:<20} = {}",          "phentsize",  header.phentsize);
   fmt::println(cout, "  {:<20} = {}",          "phnum",      header.phnum);
   fmt::println(cout, "  {:<20} = {}",          "shentsize",  header.shentsize);
   fmt::println(cout, "  {:<20} = {}",          "shnum",      header.shnum);
   fmt::println(cout, "  {:<20} = {}",          "shstrndx",   header.shstrndx);
}

void
printSectionSummary(const Rpl &rpl)
{
   fmt::println(cout, "Sections:");
   fmt::println(cout,
      "  {:<4} {:<20} {:<16} {:<8} {:<6} {:<6} {:<2} {:<4} {:<2} {:<4} {:<5}",
      "[Nr]", "Name", "Type", "Addr", "Off", "Size", "ES", "Flag", "Lk", "Info", "Align");

   for (auto i = 0u; i < rpl.sections.size(); ++i) {
      auto &section = rpl.sections[i];
      auto type = formatSHT(section.header.type);
      auto flags = formatSHF(section.header.flags);

      fmt::println(cout,
         "  [{:>2}] {:<20} {:<16} {:08X} {:06X} {:06X} {:02X} {:>4} {:>2} {:>4} {:>5}",
         i,
         section.name,
         type,
         section.header.addr.value(),
         section.header.offset.value(),
         section.header.size.value(),
         section.header.entsize.value(),
         flags,
         section.header.link,
         section.header.info,
         section.header.addralign);
   }
}

void
printFileInfo(const Rpl &rpl,
              const Section &section)
{
   auto &info = *reinterpret_cast<const elf::RplFileInfo *>(section.data.data());
   fmt::println(cout, "  {:<20} = 0x{:08X}", "version",        info.version.value());
   fmt::println(cout, "  {:<20} = 0x{:08X}", "textSize",       info.textSize.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",   "textAlign",      info.textAlign.value());
   fmt::println(cout, "  {:<20} = 0x{:08X}", "dataSize",       info.dataSize.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",   "dataAlign",      info.dataAlign.value());
   fmt::println(cout, "  {:<20} = 0x{:08X}", "loadSize",       info.loadSize.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",   "loadAlign",      info.loadAlign.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",   "tempSize",       info.tempSize.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",   "trampAdjust",    info.trampAdjust.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",   "trampAddition",  info.trampAddition.value());
   fmt::println(cout, "  {:<20} = 0x{:08X}", "sdaBase",        info.sdaBase.value());
   fmt::println(cout, "  {:<20} = 0x{:08X}", "sda2Base",       info.sda2Base.value());
   fmt::println(cout, "  {:<20} = 0x{:08X}", "stackSize",      info.stackSize.value());
   fmt::println(cout, "  {:<20} = 0x{:08X}", "heapSize",       info.heapSize.value());

   if (info.filename) {
      auto filename = section.data.data() + info.filename;
      fmt::println(cout, "  {:<20} = {}",    "filename",       filename);
   } else {
      fmt::println(cout, "  {:<20} = 0",     "filename");
   }

   fmt::println(cout, "  {:<20} = 0x{:X}",   "flags",               info.flags.value());
   fmt::println(cout, "  {:<20} = 0x{:08X}", "minSdkVersion",       info.minVersion.value());
   fmt::println(cout, "  {:<20} = {}",       "compressionLevel",    info.compressionLevel.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",   "fileInfoPad",         info.fileInfoPad.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",   "sdkVersion",          info.cafeSdkVersion.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",   "sdkRevision",         info.cafeSdkRevision.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",   "tlsModuleIndex",      info.tlsModuleIndex.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",   "tlsAlignShift",       info.tlsAlignShift.value());
   fmt::println(cout, "  {:<20} = 0x{:X}",   "runtimeFileInfoSize", info.runtimeFileInfoSize.value());

   if (info.tagOffset) {
      const char *tags = section.data.data() + info.tagOffset;
      fmt::println(cout, "  Tags:");

      while (*tags) {
         auto key = tags;
         tags += strlen(tags) + 1;
         auto value = tags;
         tags += strlen(tags) + 1;

         fmt::println(cout, "    \"{}\" = \"{}\"", key, value);
      }
   }
}

void
printRela(const Rpl &rpl,
          const Section &section)
{
   fmt::print(cout,
      "  {:<8} {:<8} {:<16} {:<8} {}\n", "Offset", "Info", "Type", "Value", "Name + Addend");

   auto &symSec = rpl.sections[section.header.link];
   auto symbols = reinterpret_cast<const elf::Symbol *>(symSec.data.data());
   auto &symStrTab = rpl.sections[symSec.header.link];

   auto relas = reinterpret_cast<const elf::Rela *>(section.data.data());
   auto count = section.data.size() / sizeof(elf::Rela);

   for (auto i = 0u; i < count; ++i) {
      auto &rela = relas[i];

      auto index = rela.info >> 8;
      auto type = rela.info & 0xff;
      auto typeName = formatRelType(type);

      auto symbol = symbols[index];
      auto name = reinterpret_cast<const char*>(symStrTab.data.data()) + symbol.name;

      fmt::println(cout,
         "  {:08X} {:08X} {:<16} {:08X} {} + {:X}",
         rela.offset.value(),
         rela.info.value(),
         typeName,
         symbol.value.value(),
         name,
         rela.addend.value());
   }
}

void
printSymTab(const Rpl &rpl,
            const Section &section)
{
   auto strTab = reinterpret_cast<const char*>(rpl.sections[section.header.link].data.data());

   fmt::println(cout,
      "  {:<4} {:<8} {:<6} {:<8} {:<8} {:<3} {}",
      "Num", "Value", "Size", "Type", "Bind", "Ndx", "Name");

   auto id = 0u;
   auto symbols = reinterpret_cast<const elf::Symbol *>(section.data.data());
   auto count = section.data.size() / sizeof(elf::Symbol);

   for (auto i = 0u; i < count; ++i) {
      auto &symbol = symbols[i];

      auto name = strTab + symbol.name;
      auto binding = symbol.info >> 4;
      auto type = symbol.info & 0xf;
      auto typeName = formatSymType(type);
      auto bindingName = formatSymBinding(binding);
      auto ndx = formatSymShndx(symbol.shndx);

      fmt::println(cout,
         "  {:>4} {:08X} {:>6} {:<8} {:<8} {:>3} {}",
         id,
         symbol.value.value(),
         symbol.size,
         typeName,
         bindingName,
         ndx,
         name);

      ++id;
   }
}

void
printRplImports(const Rpl &rpl,
                const Section &section)
{
   auto sectionIndex = getSectionIndex(rpl, section);
   auto import = reinterpret_cast<const elf::RplImport *>(section.data.data());
   fmt::println(cout, "  {:<20} = {}", "name", import->name);
   fmt::println(cout, "  {:<20} = 0x{:08X}", "signature", import->signature.value());
   fmt::println(cout, "  {:<20} = {}", "count", import->count);

   if (import->count) {
      for (auto &symSection : rpl.sections) {
         if (symSection.header.type != elf::SHT_SYMTAB) {
            continue;
         }

         auto symbols = reinterpret_cast<const elf::Symbol *>(symSection.data.data());
         auto count = symSection.data.size() / sizeof(elf::Symbol);
         auto strTab = reinterpret_cast<const char*>(rpl.sections[symSection.header.link].data.data());

         for (auto i = 0u; i < count; ++i) {
            auto &symbol = symbols[i];
            auto type = symbol.info & 0xf;

            if (symbol.shndx == sectionIndex &&
               (type == elf::STT_FUNC || type == elf::STT_OBJECT)) {
               fmt::println(cout, "    {}", strTab + symbol.name);
            }
         }
      }
   }
}

void
printRplCrcs(const Rpl &rpl,
             const Section &section)
{
   auto crcs = reinterpret_cast<const elf::RplCrc *>(section.data.data());
   auto count = section.data.size() / sizeof(elf::RplCrc);

   for (auto i = 0u; i < count; ++i) {
      fmt::println(cout, "  [{:>2}] 0x{:08X} {}", i, crcs[i].crc.value(), section.name);
   }
}

void
printRplExports(const Rpl &rpl,
                const Section &section)
{
   auto exports = reinterpret_cast<const elf::RplExport *>(section.data.data());
   auto strTab = section.data.data();
   fmt::println(cout, "  {:<20} = 0x{:08X}", "signature", exports->signature.value());
   fmt::println(cout, "  {:<20} = {}", "count", exports->count);

   for (auto i = 0u; i < exports->count; ++i) {
      // TLS exports have the high bit set in name for some unknown reason...
      auto name = strTab + (exports->exports[i].name & 0x7FFFFFFF);
      auto value = exports->exports[i].value;

      fmt::println(cout, "    0x{:08X} {}", value.value(), name);
   }
}
