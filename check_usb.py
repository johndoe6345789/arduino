from serial.tools import list_ports

ports = list_ports.comports()
if not ports:
    print("No ports found")
else:
    p = ports[0]
    print(f'Device: {p.device}')
    print(f'Description: {p.description}')
    print(f'HWID: {p.hwid}')
    print(f'VID: {p.vid}')
    print(f'PID: {p.pid}')
    print(f'Product: {p.product}')
    print(f'Manufacturer: {p.manufacturer}')
    print(f'Serial: {p.serial_number}')
    print(f'Location: {p.location}')
    print(f'Interface: {p.interface}')
    print(f'\nAll attributes:')
    print(dir(p))
