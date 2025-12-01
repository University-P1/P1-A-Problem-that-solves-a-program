from ast import TypeVar
from enum import Enum
from typing import Any, Callable, final
import customtkinter as tk

screen_scale = 100
screen_width = 16 * screen_scale
screen_height = 9 * screen_scale

shift_held: bool = False

def iota(end: int) -> list[int]:
    return list(range(end))

def on_press_key(event) -> None:
    global shift_held
    if event.keycode == 50:
        shift_held = True


def on_release_key(event) -> None:
    global shift_held
    if event.keycode == 50:
        shift_held = False


@final
class Cell(tk.CTkButton):
    def __init__(self, cellgrid, x: int, y: int):
        width = int(cellgrid.width / cellgrid.size[0])
        height = int(cellgrid.height / cellgrid.size[1])

        super().__init__(
            cellgrid,
            text="",
            border_width=1,
            border_color="black",
            width=width,
            height=height,
            command=lambda: self.button_callback(x, y)
        )

        self.type: VegType = VegType.TYPE1
        self.state: CellState = CellState.NORMAL


    def button_callback(self, x: int, y: int) -> None:
        self.master.select_button(x, y)


    def select(self) -> None:
        self.configure(border_color="red", border_width=2)
        self.update()

    def unselect(self) -> None:
        self.configure(border_color="black", border_width=1)
        self.update()


    def update(self) -> None:
        col = None
        match (self.state):
            case CellState.NORMAL:
                col = "green"
            case CellState.ONFIRE:
                col = "red"
            case CellState.BURNTOUT:
                col = "black"

        self.configure(fg_color=col)


@final
class CellGridFrame(tk.CTkFrame):
    def __init__(self, master: Any, gridsize: tuple[int, int], width: int=int(screen_width * 0.8), height: int=int(screen_height * 0.8)) -> None:  # pyright: ignore[reportExplicitAny, reportAny]
        super().__init__(master, width, height)  # pyright: ignore[reportUnknownMemberType]
        self.width: int = width;
        self.height: int = height;

        self.size: tuple[int, int] = gridsize

        self.grid_columnconfigure(iota(gridsize[0]), weight=1)  # pyright: ignore[reportUnusedCallResult]
        self.grid_rowconfigure(iota(gridsize[1]), weight=1)  # pyright: ignore[reportUnusedCallResult]

        self.cells: list[list[Cell]] = []
        self.selected: list[tuple[int, int]] = []

        for row_num in range(gridsize[0]):
            self.cells.append([])
            row = self.cells[row_num]
            for col_num in range(gridsize[1]):
                row.append(self.make_cell(row_num, col_num))
                row[col_num].update()


    def make_cell(self, x: int, y: int):
        cell = Cell(self, x, y)
        cell.grid(row=x, column=y)
        return cell


    def select_button(self, x: int, y: int) -> None:
        global shift_held
        if shift_held:
            self.select_multiple(x, y)

        else:
            self.select_single(x, y)


    def select_single(self, x: int, y: int) -> None:
        self.reset_selected()

        cell = self.cells[x][y]
        cell.select()

        self.selected.append((x, y))


    def select_multiple(self, x: int, y: int) -> None:
        if len(self.selected) == 1:
            sx = self.selected[0][0]
            sy = self.selected[0][1]

            minx = min(sx, x)
            maxx = max(sx, x)
            miny = min(sy, y)
            maxy = max(sy, y)
            for x in range(minx, maxx + 1):
                for y in range(miny, maxy + 1):
                    self.cells[x][y].select()
                    self.selected.append((x, y))

        else:
            self.select_single(x, y)


    def reset_selected(self) -> None:
        for selected in self.selected:
            self.cells[selected[0]][selected[1]].unselect()

        self.selected.clear()


    def for_all_selected(self, fn: Callable[[Cell], None]) -> None:
        for selected in self.selected:
            cell = self.cells[selected[0]][selected[1]]
            fn(cell)
            cell.update()



class VegType(Enum):
    TYPE1 = 0,
    TYPE2 = 1,


class CellState(Enum):
    NORMAL = 'N',
    ONFIRE = 'F',
    BURNTOUT = 'O',


class App(tk.CTk):
    def __init__(self):
        super().__init__()

        self.resizable(False, False)
        self.title("my app")
        self.geometry(f"{screen_width}x{screen_height}")
        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)

        self.shift_held: bool = False

        self.cell_grid = CellGridFrame(self, (13, 13))
        self.cell_grid.grid(row=0, column=0, padx=10, pady=10, sticky="nsw")

        self.frame = tk.CTkFrame(self)
        self.frame.grid(row=0, column=1, padx=10, pady=10, sticky="e")

        self.frame.grid_columnconfigure(0, weight=1)
        self.frame.grid_rowconfigure(iota(10), weight=1)

        button = tk.CTkButton(self.frame, text="onfire", command=lambda: self.set_selected_cellstate(CellState.ONFIRE), width=100, height=100)
        button.grid(row=0, column=0, padx=10, pady=10)

        button = tk.CTkButton(self.frame, text="burnt", command=lambda: self.set_selected_cellstate(CellState.BURNTOUT), width=100, height=100)
        button.grid(row=1, column=0, padx=10, pady=10)

        button = tk.CTkButton(self.frame, text="normal", command=lambda: self.set_selected_cellstate(CellState.NORMAL), width=100, height=100)
        button.grid(row=2, column=0, padx=10, pady=10)

    
    def set_selected_cellstate(self, state: CellState) -> None:
        self.cell_grid.for_all_selected(lambda cell: self.set_cellstate(cell, state))


    def set_cellstate(self, cell: Cell, state: CellState) -> None:
        cell.state = state



app = App()
app.bind("<KeyPress>", on_press_key)
app.bind("<KeyRelease>", on_release_key)
app.mainloop()


