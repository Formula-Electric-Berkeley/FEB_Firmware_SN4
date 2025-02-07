import csv
from typing import Callable

# Constants

CAN_ID_CSV_FILENAME = "FEB_CAN_ID.csv"
CAN_STATIC_ID_CSV_FILENAME = "FEB_CAN_Static_ID.csv"

C_HEADER_FILENAME = "FEB_CAN_ID.h"
C_HEADER_GUARD = "INC_FEB_CAN_ID_H_"
C_CAN_MESSAGE_PREFIX = "FEB_CAN_ID_"

PYTHON_FILENAME = "feb_can_id.py"

# Global Variables

CAN_static_id_set = set()
CAN_id = 0

# CAN

def get_available_CAN_id() -> int:
    global CAN_id
    while CAN_id in CAN_static_id_set:
        CAN_id += 1
    CAN_id += 1
    return CAN_id - 1

def format_CAN_message_name(name: str) -> str:
    return C_CAN_MESSAGE_PREFIX + "_".join(name.upper().split(" "))
    
# C Header File

def is_c_comment(csv_row: list[str]) -> bool:
    return "//" in csv_row[0]

def c_header_comment(s: str) -> str:
    return f"// {'*' * 8} {s} {'*' * 8}"

def c_define_macro(name: str, value: "hex") -> str:
    return f"#define {name} {value}"

def generate_c_header_file(filename: str, CAN_id_data: list[dict], CAN_static_id_data: list[dict]) -> None:
    def write_CAN_id_to_file(CAN_id_data: list[dict]):
        for CAN_message in CAN_id_data:
            if "comment" in CAN_message:
                c_header_file.write("\n")
                c_header_file.write(CAN_message["comment"] + "\n")
            else:
                c_header_file.write(c_define_macro(CAN_message["name"], CAN_message["id"]) + "\n")

    with open(filename, "w") as c_header_file:
        # Open header guard
        c_header_file.write(f"#ifndef {C_HEADER_GUARD}\n")
        c_header_file.write(f"#define {C_HEADER_GUARD}\n")

        # Write CAN IDs
        c_header_file.write("\n")
        c_header_file.write(c_header_comment("Static CAN IDs") + "\n")
        write_CAN_id_to_file(CAN_static_id_data)

        c_header_file.write("\n")
        c_header_file.write(c_header_comment("Dynamic CAN IDs") + "\n")
        write_CAN_id_to_file(CAN_id_data)

        # Close header guard
        c_header_file.write("\n")
        c_header_file.write(f"#endif /* {C_HEADER_GUARD} */\n")

# Python file

def python_header_comment(s: str) -> str:
    return f"# {'*' * 8} {s} {'*' * 8}"

def python_assign_macro(name: str, value: "hex") -> str:
    return f"{name} = {value}"

def generate_python_file(filename: str, CAN_id_data: list[dict], CAN_static_id_data: list[dict]) -> None:
    def write_CAN_id_to_file(CAN_id_data: list[dict]):
        for CAN_message in CAN_id_data:
            if "comment" in CAN_message:
                python_file.write("\n")
                python_file.write(f"# {CAN_message['comment'][3:]}\n")
            else:
                python_file.write(python_assign_macro(CAN_message["name"].replace(C_CAN_MESSAGE_PREFIX, ''), 
                                                      CAN_message["id"]) + "\n")

    with open(filename, "w") as python_file:
        # Static CAN IDs
        python_file.write(python_header_comment("Static CAN IDs") + "\n")
        write_CAN_id_to_file(CAN_static_id_data)
        python_file.write("\n")

        # Dynamic CAN IDs
        python_file.write(python_header_comment("Dynamic CAN IDs") + "\n")
        write_CAN_id_to_file(CAN_id_data)

# CSV File

def is_csv_comment(csv_row: list[str]) -> bool:
    return "#" in csv_row[0]

def read_csv_data(filename: str, row_func: Callable[[list[str]], dict[str, str]]) -> list[str]:
    CAN_id_data = []
    with open(filename) as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=",")
        for row in csv_reader:
            if row and not is_csv_comment(row):
                CAN_id_data.append(row_func(row))
    return CAN_id_data

def process_CAN_id_csv_row(row: list[str]) -> dict[str, str]:
    if is_c_comment(row):
        CAN_message_data = {"comment": row[0]}
    else:
        CAN_message_data = {
            "name": format_CAN_message_name(row[0]),
            "id": hex(get_available_CAN_id())
        }
    return CAN_message_data

def process_CAN_static_id_csv_row(row: list[str]) -> dict[str, str]:
    if is_c_comment(row):
        CAN_message_data = {"comment": row[0]}
    else:
        CAN_Id = int(row[1], 0)
        CAN_message_data = {
            "name": format_CAN_message_name(row[0]),
            "id": hex(CAN_Id)
        }
        CAN_static_id_set.add(CAN_Id)
    return CAN_message_data

def main() -> None:
    csv_static_id_data = read_csv_data(CAN_STATIC_ID_CSV_FILENAME, process_CAN_static_id_csv_row)
    csv_id_data = read_csv_data(CAN_ID_CSV_FILENAME, process_CAN_id_csv_row)
    generate_c_header_file(C_HEADER_FILENAME, csv_id_data, csv_static_id_data)
    generate_python_file(PYTHON_FILENAME, csv_id_data, csv_static_id_data)

if __name__ == "__main__":
    main()
