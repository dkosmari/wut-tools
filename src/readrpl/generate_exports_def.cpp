#include "generate_exports_def.h"

#include <fmt/base.h>
#include <fmt/ostream.h>
#include <fstream>
#include <iostream>
#include <unordered_set>

using std::cerr;

static const std::unordered_set<std::string>
sExportBlacklist = {
   "__get_eh_globals",
   "__get_eh_init_block",
   "__get_eh_mem_manage",
   "__get_eh_store_globals",
   "__get_eh_store_globals_tdeh",
   "__gh_errno_ptr",
   "__gh_get_errno",
   "__gh_iob_init",
   "__gh_lock_init",
   "__gh_set_errno",
   "__ghsLock",
   "__ghsUnlock",
   "__ghs_at_exit",
   "__ghs_at_exit_cleanup",
   "__ghs_flock_create",
   "__ghs_flock_destroy",
   "__ghs_flock_file",
   "__ghs_flock_ptr",
   "__ghs_ftrylock_file",
   "__ghs_funlock_file",
   "__ghs_mtx_dst",
   "__ghs_mtx_init",
   "__ghs_mtx_lock",
   "__ghs_mtx_unlock",
   "__tls_get_addr",
   "memclr",
   "memcpy",
   "memmove",
   "memset",
   "__atexit_cleanup",
   "__cpp_exception_cleanup_ptr",
   "__cpp_exception_init_ptr",
   "__gh_FOPEN_MAX",
   "__ghs_cpp_locks",
   "__stdio_cleanup",
   "_iob",
   "_iob_lock",
   "environ",
   "errno",
};

static bool
inBlacklist(const char *name)
{
   return sExportBlacklist.contains(name);
}

bool
generateExportsDef(const Rpl &rpl,
                   const std::string &rplName,
                   const std::filesystem::path &outFileName)
{
   std::ofstream out{outFileName};
   if (!out.is_open()) {
      fmt::println(cerr, "Failed to open \"{}\" for writing!", outFileName.string());
      return false;
   }

   fmt::println(out, ":NAME {}", rplName);

   for (auto &section : rpl.sections) {
      if (section.header.type == elf::SHT_RPL_EXPORTS) {
         auto exports = reinterpret_cast<const elf::RplExport *>(section.data.data());
         auto strTab = section.data.data();

         if (section.header.flags & elf::SHF_EXECINSTR) {
            fmt::println(out, "\n:TEXT");
         } else {
            fmt::println(out, "\n:DATA");
         }

         for (auto i = 0u; i < exports->count; ++i) {
            if (exports->exports[i].name & 0x80000000) {
               // Skip TLS exports for now.
               continue;
            }

            auto name = strTab + (exports->exports[i].name & 0x7FFFFFFF);

            if (inBlacklist(name)) {
               fmt::print(out, "//");
            }

            fmt::println(out, "{}", name);
         }

      }
   }

   return true;
}
