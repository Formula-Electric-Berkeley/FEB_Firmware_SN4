import serial, serial.tools.list_ports, serial.tools.list_ports_common
import sys

PortType = serial.tools.list_ports_common.ListPortInfo

class SerialConnection:
    BAUDRATE = 115200
    BYTESIZE = serial.SEVENBITS
    PARITY = serial.PARITY_EVEN
    STOPBITS = serial.STOPBITS_ONE

    def __init__(self):
        ports: list[PortType] = self._get_active_ports()
        self._display_active_ports(ports)
        selected_port: str = self._select_active_port(ports)
        self.connection: serial.Serial = self._connect(selected_port)
    
    def read_data(self) -> None:
        self.connection.readline()
        message = ""
        while True:
            try:
                message: bytes = self.connection.readline()
                print(message)
            except KeyboardInterrupt:
                sys.exit(0)
            except:
                print(message)

    def _connect(self, port: str) -> serial.Serial:
        try:
            serial_connection: serial.Serial = serial.Serial(port,
                                                             SerialConnection.BAUDRATE, SerialConnection.BYTESIZE,
                                                             SerialConnection.PARITY, SerialConnection.STOPBITS)
            return serial_connection
        except serial.SerialException as err:
            raise Exception("Cannot connect to serial port.")
    
    def _display_active_ports(self, ports: list[PortType]) -> None:
        ports_str: list[str] = [p[0] for p in ports]
        print(f"Active ports: {ports_str}")

    def _get_active_ports(self) -> list[PortType]:
        ports: list[PortType] = serial.tools.list_ports.comports()
        if len(ports) == 0:
            raise Exception("No active ports.")
        return ports

    def _select_active_port(self, ports: list[PortType]) -> str:
        port_index: int = -1
        while not 1 <= port_index <= len(ports):
            try:
                port_index = int(input(f"Choose port [1, {len(ports)}]: "))
            except:
                pass
        return ports[port_index - 1][0]

def main() -> None:
    sc: SerialConnection = SerialConnection()
    sc.read_data()

if __name__ == "__main__":
    main()