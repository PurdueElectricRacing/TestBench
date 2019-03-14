import serial
import datetime
import warnings
import atexit

ACK = b'\x06'  # CANDapter successful acknowledge


class CANFrame:
    def __init__(self, m_id, m_timestamp, m_data):
        self.id = m_id
        self.data = m_data
        self.timestamp = m_timestamp
        self.length = int(len('{:x}'.format(self.data)) / 2)

        bits = len('{:x}'.format(self.data))
        if bits % 2 == 1 or self.length > 8:
            raise ValueError('Invalid data length provided to CAN message')


class GenericCANAdapterDevice:
    def __init__(self):
        pass

    def sendSerialMessage(self, message):
        pass

    def sendCANMessage(self, id, message):
        pass

    def readCANMessage(self):
        pass


class CANDapterDevice(GenericCANAdapterDevice):
    def __init__(self,
                 port='/dev/ttyUSB0',
                 baudrate=2500000,
                 timeout=None,
                 debug=False):

        super().__init__()
        # Set bitrate to 500Kbit and open CANDapter
        # https://www.ewertenergy.com/products/candapter/downloads/candapter_manual.pdf
        self.canDapterDevice = serial.Serial(port, baudrate, timeout=timeout)
        self.sendSerialMessage('S6')
        self.sendSerialMessage('O')

        # Always close the connection at the end,
        # so no need for the user to do it manually
        atexit.register(self.closeConnection)

    def sendSerialMessage(self, message):
        self.canDapterDevice.write('{}\r'.format(message).encode())
        self.checkCANDapterResponse()

    def __readSerialMessage(self):
        return self.canDapterDevice.read()

    def sendCANMessage(self, canFrame):
        self.sendSerialMessage('T{id:x}{length:d}{data:x}'.format(
            id=canFrame.id,
            length=canFrame.length,
            data=canFrame.data
        ))

    def readCANMessage(self):
        message = self.canDapterDevice.read_until('\r').replace('\r', '')[1:]

        m_id = message[0:3]
        m_len = message[3:4]
        m_message = message[4:-4]
        m_time_stamp = str(datetime.datetime.now())

        canFrame = CANFrame(m_id, m_time_stamp, m_message)
        assert m_len == canFrame.length
        return canFrame

    def checkCANDapterResponse(self):
        return_message = self.__readSerialMessage()
        if return_message != ACK:
            warnings.warn(
                'CANDapter did not return ACK, instead returned {}'
                .format(return_message)
            )

    def closeConnection(self):
        self.canDapterDevice.write(b'C\r')
