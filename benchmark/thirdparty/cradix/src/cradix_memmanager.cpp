#include <cradix_memmanager.h>
#include <cradix_memstats.h>

void CRadix::MemManager::statistics(MemStats *stats) const {
  assert(stats);
  stats->reset();
}
