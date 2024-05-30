table_str = "const uint16_t crc10Table[256] = {"

CRC10_POLY = 0x48F

for i in range(256):
    remainder = i << 2
    for bit in range (8, 0, -1):
        if remainder & 0x200:
            remainder = ((remainder << 1))
            remainder = (remainder ^ CRC10_POLY)
        else:
            remainder = ((remainder << 1))
    table_str += (hex(remainder & 0x3FF))
    table_str += ", "
    if (i + 1) % 11 == 0:  # Add a newline every 11 entries for readability
        table_str += "\n"

table_str = table_str[0:-2] + "};"


with open("CRC10_Table/table.txt", "w") as f:
    f.write(table_str)
