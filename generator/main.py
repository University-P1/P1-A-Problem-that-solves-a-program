#!/usr/bin/env python3
from enum import Enum
from io import BufferedReader
from typing import Callable, TypeVar, final, TYPE_CHECKING, override
import customtkinter as tk
from spinbox import Spinbox

from sys import argv, stdout, stderr

if TYPE_CHECKING:
    from _typeshed import SupportsRichComparisonT
else:
    SupportsRichComparisonT = TypeVar("SupportsRichComparisonT")


screen_scale = 100
screen_width = 16 * screen_scale
screen_height = 9 * screen_scale

shift_held: bool = False


class ViewMode(Enum):
    STATE = 0,
    TYPE = 1,
    MOISTURE = 2,


viewmode: ViewMode = ViewMode.STATE


def iota(end: int) -> list[int]:
    return list(range(end))

def on_press_key(event) -> None:
    global shift_held
    if event.keycode == 50 or event.keycode == 943782142:
        shift_held = True


def on_release_key(event) -> None:
    global shift_held
    if event.keycode == 50 or event.keycode == 939587838:
        shift_held = False


def clamp(val: SupportsRichComparisonT, min_val: SupportsRichComparisonT, max_val: SupportsRichComparisonT) -> SupportsRichComparisonT:
    return max(min_val, min(val, max_val))



class VegType(Enum):
    BROADLEAVES = 'B',
    SHRUBS = 'S',
    GRASSLAND = 'G',
    FIREPRONE = 'F',
    AGROFORESTRY = 'A',
    NOTFIREPRONE = 'N',

    def get(self) -> str:
        return str(self.value[0])


def veg_type_from_str(input: str) -> VegType:
    for type in VegType:
        if input == type.get():
            return type

    _ = stderr.write(f"Invalid VegType string: {input}")
    exit(1)


class CellState(Enum):
    NORMAL = 'N'
    ONFIRE = 'F'
    BURNTOUT = 'O'

    def get(self) -> str:
        return str(self.value[0])


def cell_state_from_str(input: str) -> CellState:
    for state in CellState:
        if input == state.get():
            return state

    _ = stderr.write(f"Invalid CellState string: {input}")
    exit(1)


@final
class Cell(tk.CTkButton):
    def __init__(self, cellgrid,
                 x: int,
                 y: int,
                 command: Callable[[int, int], None]):
        width = int(cellgrid.winfo_reqwidth() / cellgrid.size[0])
        height = int(cellgrid.winfo_reqheight() / cellgrid.size[1])

        min_len = min(width, height)

        self.x = x
        self.y = y
        self.command = command

        super().__init__(
            cellgrid,
            text="",
            border_width=1,
            border_color="black",
            width=min_len,
            height=min_len,
            corner_radius=0,
            command=self.callback
        )

        self.type: VegType = VegType.NOTFIREPRONE
        self.state: CellState = CellState.NORMAL
        self.moisture: int = 80

    def callback(self) -> None:
        self.command(self.x, self.y)


    def select(self) -> None:
        self.configure(border_color="yellow", border_width=2)
        self.update_cell()

    def unselect(self) -> None:
        self.configure(border_color="black", border_width=1)
        self.update_cell()


    @override
    def __str__(self) -> str:
        return f"""
            {{
                .state = {self.state}
                .type = {self.type}
                .moisture = {self.moisture}
            }}
        """


    def update_cell(self) -> None:
        col = None
        match viewmode:
            case ViewMode.STATE:
                match self.state:
                    case CellState.NORMAL:
                        col = "green"
                    case CellState.ONFIRE:
                        col = "red"
                    case CellState.BURNTOUT:
                        col = "black"

            case ViewMode.TYPE:
                match self.type:
                    case VegType.BROADLEAVES:
                        col = "blue"

                    case VegType.SHRUBS:
                        col = "dark green"

                    case VegType.GRASSLAND:
                        col = "light green"

                    case VegType.FIREPRONE:
                        col = "red"

                    case VegType.AGROFORESTRY:
                        col = "yellow"

                    case VegType.NOTFIREPRONE:
                        col = "grey"

            case ViewMode.MOISTURE:
                moist_hex = f"{self.moisture:x}"
                while len(moist_hex) < 2:
                    moist_hex = "0" + moist_hex

                _ = stderr.write(f"moist_val: {moist_hex}\n")

                col = f"#1111" + moist_hex
                _ = stderr.write(f"col: {col}\n")


        self.configure(fg_color=col)

    def set_type(self, type: VegType) -> None:
        self.type = type

    def set_state(self, state: CellState) -> None:
        self.state = state

    def set_moisture(self, moist: int) -> None:
        self.moisture = clamp(moist, 0, 100)
        _ = stderr.write(f"moisture: {self.moisture}\n")


    def serialize(self) -> str:
        out: str = ""
        out += self.state.get() + ","
        out += self.type.get() + ","
        out += str(self.moisture) + ","

        return out

    def update_from_serialized(self, serialized: bytes) -> None:
        data = serialized.split(b",")
        
        self.state = cell_state_from_str(data[0].decode())
        self.type = veg_type_from_str(data[1].decode())
        self.moisture = int(data[2])


@final
class CellGridFrame(tk.CTkFrame):
    def __init__(self, master,
                 gridsize: tuple[int, int],
                 command: Callable[[list[Cell]], None],
                 in_file: BufferedReader | None) -> None:
        width: int = int(master.winfo_width() * 0.85)
        height: int = int(master.winfo_height() * 0.85)

        min_len = min(width, height)
        super().__init__(master, width=min_len, height=min_len)

        self.size: tuple[int, int] = gridsize
        self.command = command

        _ = self.grid_columnconfigure(iota(gridsize[0]), weight=1)
        _ = self.grid_rowconfigure(iota(gridsize[1]), weight=1)

        self.cells: list[list[Cell]] = []
        self.selected: list[Cell] = []

        self.update()
        for row_num in range(gridsize[0]):
            self.cells.append([])
            row = self.cells[row_num]
            for col_num in range(gridsize[1]):
                row.append(self.make_cell(row_num, col_num, in_file))
                row[col_num].update_cell()


    def make_cell(self, x: int, y: int, reader: BufferedReader | None):
        cell = Cell(self, x, y, self.select_button)
        cell.grid(row=x, column=y)

        if reader is not None:
            cell.update_from_serialized(reader.readline())

        return cell


    def select_button(self, x: int, y: int) -> None:
        global shift_held
        if shift_held:
            self.select_multiple(x, y)

        else:
            self.select_single(x, y)

        
        # deduplicate, keeping order
        unique: set[Cell] = set([])
        new_list: list[Cell] = []
        for cell in self.selected:
            if cell not in unique:
                new_list.append(cell)

        self.selected = new_list

        self.command(self.selected)


    def select_single(self, x: int, y: int) -> None:
        self.reset_selected()

        cell = self.cells[x][y]
        cell.select()

        self.selected.append(cell)


    def select_multiple(self, x: int, y: int) -> None:
        if len(self.selected) == 0:
            self.select_single(x, y)
            return


        first = self.selected[0]
        sx = first.x
        sy = first.y

        x_is_backwards = x < sx
        y_is_backwards = y < sy

        x_range = range(sx, x - 1, -1) if x_is_backwards else range(sx, x + 1)
        y_range = range(sy, y - 1, -1) if y_is_backwards else range(sy, y + 1)

        for x_pos in x_range:
            for y_pos in y_range:
                # Don't append the currently selected cell twice!
                if x_pos == sx and y_pos == sy:
                    continue

                # Don't store it in a variable, because python sucks, and overrides it :(
                self.cells[x_pos][y_pos].select()
                self.selected.append(self.cells[x_pos][y_pos])


        # Set the selected cell as the first element
        self.selected.remove(self.cells[x][y])
        self.selected.insert(0, self.cells[x][y])


    def reset_selected(self) -> None:
        for selected in self.selected:
            selected.unselect()

        self.selected.clear()


    def for_all_selected(self, fn: Callable[[Cell], None]) -> None:
        for cell in self.selected:
            fn(cell)
            cell.update_cell()

    
    def multi_select(self) -> bool:
        return len(self.selected) > 1


@final
class SettingsFrame(tk.CTkFrame):
    def __init__(self, master, cellgrid: CellGridFrame):
        super().__init__(master)
        _ = self.grid_columnconfigure(0, weight=1)
        _ = self.grid_rowconfigure(iota(4), weight=1)

        self.cellgrid: CellGridFrame = cellgrid

        self.number_settings = tk.CTkFrame(self)
        self.number_settings.grid(row=0, column=0)
        _ = self.number_settings.grid_columnconfigure(0, weight=1)
        _ = self.number_settings.grid_rowconfigure(iota(6), weight=1)


        text = tk.CTkEntry(self.number_settings, height=32, justify="center")
        text.grid(row=0, column=0, pady=(0,10))
        text.insert(0, "moisture")
        text.configure(state="disabled")

        self.moisture_meter = Spinbox(self.number_settings, command=self.moisture_callback)
        self.moisture_meter.grid(row=1, column=0, pady=10, padx=10)
        self.moisture_meter.set(0)

        self.onfire = tk.CTkButton(self, text="onfire", command=lambda: self.set_selected_cellstate(CellState.ONFIRE), width=100, height=100)
        self.onfire.grid(row=1, column=0, padx=10, pady=10)

        self.burnt = tk.CTkButton(self, text="burnt", command=lambda: self.set_selected_cellstate(CellState.BURNTOUT), width=100, height=100)
        self.burnt.grid(row=2, column=0, padx=10, pady=10)

        self.normal = tk.CTkButton(self, text="normal", command=lambda: self.set_selected_cellstate(CellState.NORMAL), width=100, height=100)
        self.normal.grid(row=3, column=0, padx=10, pady=10)


    def update_selected(self, selected: list[Cell]) -> None:
        if len(selected) == 0:
            return

        if len(selected) > 1:
            self.moisture_meter.set(0)
        else:
            self.moisture_meter.set(selected[0].moisture)


    def set_selected_cellstate(self, state: CellState) -> None:
        self.cellgrid.for_all_selected(lambda cell: cell.set_state(state))


    def moisture_callback(self, moist: int) -> None:
        # If we have multiple cells selected at once, we increment by the amount
        if self.cellgrid.multi_select():
            self.cellgrid.for_all_selected(lambda cell: cell.set_moisture(cell.moisture + moist))
            self.moisture_meter.set(0) # Reset the value, so we don't increment twice
        # Only 1 selected, we just set the value directly:
        else:
            self.cellgrid.for_all_selected(lambda cell: cell.set_moisture(moist))


@final
class ViewModeFrame(tk.CTkFrame):
    def __init__(self, master, command: Callable[[ViewMode], None]):
        super().__init__(master)
        self.command = command

        for i, mode in enumerate(ViewMode):
            _ = self.grid_columnconfigure(i, weight=1)
            button = self.make_button(mode)
            button.grid(row=0, column=i, padx=10, pady=10)


        button = tk.CTkButton(self, height=80, text="Export", fg_color="red", command=master.export)
        button.grid(row=0, column=len(ViewMode), padx=10, pady=10)


    def make_button(self, mode: ViewMode) -> tk.CTkButton:
        return tk.CTkButton(self, text=mode.name, command=lambda: self.set_mode(mode), height=60)

    def set_mode(self, mode: ViewMode) -> None:
        global viewmode
        viewmode = mode
        self.command(mode)


@final
class App(tk.CTk):
    def __init__(self):
        super().__init__()

        self.resizable(False, False)
        self.title("my app")
        self.geometry(f"{screen_width}x{screen_height}")
        _ = self.grid_rowconfigure(0, weight=1)
        _ = self.grid_rowconfigure(1, weight=3)
        _ = self.grid_columnconfigure(iota(3), weight=1)
        self.update()

        try:
            size = (int(argv[1]), int(argv[2])) if len(argv) >= 3 else (50, 50)
        except:
            _ = stderr.write("Invalid parameters!!")
            exit(1)

        in_file = open(argv[3], mode="rb") if len(argv) == 4 else None

        self.cellgrid = CellGridFrame(self, size, self.update_selected, in_file)
        self.cellgrid.grid(row=0, column=0, rowspan=2, padx=10, pady=10, sticky="sw")

        self.viewmodes = ViewModeFrame(self, self.update_viewmode)
        self.viewmodes.grid(row=0, column=0, columnspan=3, padx=10, pady=10, sticky="ne")

        self.veg_types_frame = tk.CTkFrame(self)
        self.veg_types_frame.grid(row=1, column=1, padx=10, pady=10, sticky="e")

        for i, veg_type in enumerate(VegType):
            _ = self.veg_types_frame.grid_rowconfigure(i, weight=1)
            button = self.make_veg_type_button(veg_type)
            button.grid(row=i, column=0, padx=10, pady=10)


        self.settings = SettingsFrame(self, self.cellgrid)
        self.settings.grid(row=1, column=2, padx=10, pady=10, sticky="e")


    def export(self) -> None:
        writer = stdout.buffer
        for row in self.cellgrid.cells:
            _ = writer.writelines([(cell.serialize() + "\n").encode() for cell in row])

        _ = writer.flush()

        exit(0)


    def make_veg_type_button(self, veg_type: VegType) -> tk.CTkButton:
        return tk.CTkButton(self.veg_types_frame, text=veg_type.name, command=lambda: self.set_veg_type(veg_type), height=60)


    def set_veg_type(self, type: VegType) -> None:
        self.cellgrid.for_all_selected(lambda cell: cell.set_type(type))


    def update_selected(self, selected: list[Cell]) -> None:
        self.settings.update_selected(selected)


    def update_viewmode(self, mode: ViewMode) -> None:
        _ = mode
        for row in self.cellgrid.cells:
            for cell in row:
                cell.update_cell()



if __name__ == "__main__":
    app = App()
    _ = app.bind("<KeyPress>", on_press_key)
    _ = app.bind("<KeyRelease>", on_release_key)
    _ = app.bind("<Button-3>", lambda _: app.cellgrid.reset_selected())
    app.mainloop()


