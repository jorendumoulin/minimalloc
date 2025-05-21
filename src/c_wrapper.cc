#include "c_wrapper.h"
#include "minimalloc.h"
#include "solver.h"
#include <vector>

using namespace minimalloc;

extern "C" {

struct SolverHandle {
  Solver *solver;
};

struct ProblemHandle {
  Problem *problem;
};

struct SolutionHandle {
  Solution *solution;
};

ProblemHandle *Problem_Create(CBuffer *cbuffers, size_t count,
                              int64_t capacity) {
  auto handle = new ProblemHandle;

  // Initialize buffers from cbuffers
  std::vector<Buffer> buffers;

  for (int i = 0; i < count; i++) {
    Buffer buffer;
    buffer.id = cbuffers[i].id;
    buffer.lifespan = {cbuffers[i].start_time, cbuffers[i].end_time};
    buffer.size = cbuffers[i].size;
    buffer.alignment = cbuffers[i].alignment;
    buffers.push_back(buffer);
  }

  handle->problem = new Problem;
  handle->problem->capacity = capacity;
  handle->problem->buffers = buffers;

  return handle;
}

void Problem_Destroy(ProblemHandle *handle) {
  delete handle->problem;
  delete handle;
}

SolverHandle *Solver_Create() {
  auto handle = new SolverHandle;
  handle->solver = new Solver();
  return handle;
}

void Solver_Destroy(SolverHandle *handle) {
  delete handle->solver;
  delete handle;
}

int Solver_Solve(SolverHandle *s, ProblemHandle *p, SolutionHandle *sol) {
  s->solver = new Solver();
  absl::StatusOr<Solution> solution = s->solver->Solve(*(p->problem));
  if (!solution.ok()) {
    return -1; // Error
  }
  sol->solution = new Solution(std::move(solution.value()));
  return 0;
}

SolutionHandle *Solution_Create() {
  auto handle = new SolutionHandle;
  return handle;
}

void Solution_Destroy(SolutionHandle *handle) {
  delete handle->solution;
  delete handle;
}

CSolution Solution_GetCSolution(SolutionHandle *handle) {
  CSolution out;
  out.offsets = handle->solution->offsets.data();
  out.size = handle->solution->offsets.size();
  return out;
}
}
