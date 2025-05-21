import ctypes
import os
from ctypes import POINTER, c_char_p, c_int64, c_size_t, c_void_p
from dataclasses import dataclass


class _CBuffer(ctypes.Structure):
    _fields_ = [
        ("id", c_char_p),
        ("start_time", c_int64),
        ("end_time", c_int64),
        ("size", c_int64),
        ("alignment", c_int64),
    ]


class _CSolution(ctypes.Structure):
    _fields_ = [
        ("data", POINTER(c_int64)),
        ("size", c_size_t),
    ]


_lib_path = os.path.join(os.path.dirname(__file__), "libminimalloc_api.so")
_lib = ctypes.CDLL(_lib_path)

_lib.Problem_Create.argtypes = [POINTER(_CBuffer), c_size_t, c_int64]
_lib.Problem_Create.restype = c_void_p
_lib.Problem_Destroy.argtypes = [c_void_p]
_lib.Problem_Destroy.restype = None

_lib.Solver_Create.argtypes = []
_lib.Solver_Create.restype = c_void_p
_lib.Solver_Destroy.argtypes = [c_void_p]
_lib.Solver_Destroy.restype = None
_lib.Solver_Solve.argtypes = [c_void_p, c_void_p, c_void_p]
_lib.Solver_Solve.restype = int


_lib.Solution_Create.argtypes = []
_lib.Solution_Create.restype = c_void_p
_lib.Solution_Destroy.argtypes = [c_void_p]
_lib.Solution_Destroy.restype = None
_lib.Solution_GetCSolution.argtypes = [c_void_p]
_lib.Solution_GetCSolution.restype = _CSolution


@dataclass
class Buffer:
    id: str
    start_time: int
    end_time: int
    size: int
    alignment: int

    def to_cbuffer(self) -> _CBuffer:
        return _CBuffer(
            id=self.id.encode("utf-8"),
            start_time=self.start_time,
            end_time=self.end_time,
            size=self.size,
            alignment=self.alignment,
        )


class Solution(list[int]):
    @classmethod
    def from_csolution(cls, csolution: _CSolution) -> "Solution":
        return cls([int(csolution.data[i]) for i in range(csolution.size)])


class MiniMallocError(Exception): ...


@dataclass
class Problem:
    buffers: list[Buffer]
    capacity: int

    def solve(self) -> Solution:
        # Create Problem:
        buffers_c = (_CBuffer * len(self.buffers))(
            *(x.to_cbuffer() for x in self.buffers)
        )
        problem = _lib.Problem_Create(buffers_c, len(self.buffers), self.capacity)

        # Create Solver:
        solver = _lib.Solver_Create()

        # Create Solution:
        solution = _lib.Solution_Create()

        # Solve:
        success = _lib.Solver_Solve(solver, problem, solution)
        if success != 0:
            raise MiniMallocError()

        # Convert to Solution
        csolution = _lib.Solution_GetCSolution(solution)

        return Solution.from_csolution(csolution)
