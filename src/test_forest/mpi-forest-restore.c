#include "grid/quadtree.h"
#include "utils.h"

scalar value[];

int main() {
  dimensions(3, 2);
  init_grid(12);
  foreach()
    value[] = x + 2.*y;
  boundary({value});

  scalar index[];
  double last = z_indexing(index, true);
  long cells = 0;
  foreach(reduction(+:cells)) {
    cells++;
    assert(index[] >= 0.);
  }
  if (pid() == 0)
    assert(last == cells - 1);

  // Immediate restore also checks that dump() finishes the rank-zero rename.
  dump("forest-restore.dump");
  foreach()
    value[] = -1.;
  bool restored = restore("forest-restore.dump");
  assert(restored);
  foreach()
    assert(fabs(value[] - x - 2.*y) < 1e-12);

#if _MPI
  MPI_Barrier(MPI_COMM_WORLD);
#endif
  if (pid() == 0) {
    remove("forest-restore.dump");
    fprintf(stderr, "mpi-forest-restore: ok\n");
  }
}
