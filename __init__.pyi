"""
Pybind11 example plugin
-----------------------

.. currentmodule:: scikit_build_example

.. autosummary::
    :toctree: _generate

    add
    subtract
"""

def plot_signal(values: list[float]) -> None:
    """
    Rysuje wykres sygnału
    """

def generate_sin_signal(samples: int, freq: float) -> list[float]:
    """
    Generuje sygnał sin
    """
    

def generate_cos_signal(samples: int, freq: float) -> list[float]:
    """
    Generuje sygnał cos
    """

def generate_square_signal(samples: int, freq: float) -> list[float]:
    """
    Generuje sygnał kwadratowy
    """

def generate_pilo_signal(samples: int, freq: float) -> list[float]:
    """
    Generuje sygnał piłokształtny
    """

def filter_1d(values: list[float]) -> list[float]:
    """
    Filtracja 1D
    """

def filter_2d(values: list[list[float]]) -> list[list[float]]:
    """
    Filtracja 2D
    """

def dft(values: list[float]) -> list[complex[float]]:
    """
    DFT
    """

def idft(values: list[complex[float]]) -> list[float]:
    """
    IDFT
    """

def interpolacja(values: list[list[float]], new_rows: int, new_cols: int) -> list[list[float]]:
    """
    Interpolacja
    """