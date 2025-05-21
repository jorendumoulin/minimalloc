#ifndef MINIMALLOC_C_WRAPPER_H_
#define MINIMALLOC_C_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef struct SolverHandle SolverHandle;
typedef struct ProblemHandle ProblemHandle;
typedef struct SolutionHandle SolutionHandle;

// C-compatible Buffer representation
typedef struct {
  const char *id;
  int64_t start_time;
  int64_t end_time;
  int64_t size;
  int64_t alignment;
} CBuffer;

// C-compatible Solution representation
typedef struct {
  const int64_t *offsets;
  size_t size;
} CSolution;

ProblemHandle *Problem_Create(CBuffer *buffers, size_t count, int64_t capacity);
void Problem_Destroy(ProblemHandle *);

SolverHandle *Solver_Create();
void Solver_Destroy(SolverHandle *);
int Solver_Solve(SolverHandle *, ProblemHandle *, SolutionHandle *);

SolutionHandle *Solution_Create();
void Solution_Destroy(SolutionHandle *);
CSolution Solution_GetCSolution(SolutionHandle *);

#ifdef __cplusplus
}
#endif

#endif // MINIMALLOC_C_WRAPPER_H_
