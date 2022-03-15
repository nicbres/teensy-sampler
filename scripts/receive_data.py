import logging
import os
import socket

import matplotlib.animation as animation
import matplotlib.pyplot as plt
import numpy as np

logging.basicConfig(
    # filename="receive_data.log",
    format="%(levelname)s: %(message)s",
    level=logging.INFO,
)

COMMANDS = {
    0x0: "NONE",
    0x1: "START",
    0xC: "DATA",
}

UDP_IP = "10.0.0.2"
UDP_PORT = 42142

logging.debug(f"IP: {UDP_IP}")
logging.debug(f"Port: {UDP_PORT}")

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))
logging.debug("Socket connected")


def unpack_words(
    byte_string,
    word_length=4,
):
    if len(byte_string) % word_length:
        raise ValueError("byte string is not integer multiple of word length")

    words = []
    for index in range(0, len(byte_string) // word_length):
        start_index = index * word_length
        stop_index = (index + 1) * word_length
        words.append(byte_string[start_index:stop_index][::-1])

    return words


def convert_word_to_integer(
    word,
):
    integer = 0
    for index, byte_elem in enumerate(word[::-1]):
        integer += 16 ** (2 * index) * byte_elem

    return integer


# setup plots
frame_length = 250000
# frame_length = 30
fig, ax = plt.subplots()

x = np.arange(frame_length)
y = np.zeros(frame_length)

ax.grid()

(lines,) = ax.plot(x, y)
plt.pause(0.001)
fig.canvas.draw()
plt.show(block=False)

index = 0
while True:
    data, addr = sock.recvfrom(500)  # buffer size is 1024 bytes

    words = unpack_words(byte_string=data)
    header = convert_word_to_integer(word=words[0])
    payload = convert_word_to_integer(word=words[1])
    logging.debug(f"Packet {index:3}: | {COMMANDS[header]:>8} | {payload:>8} |")

    y[index] = payload

    index = (index + 1) % frame_length
    if index == 0:
        fig = plt.gcf()

        lines.set_xdata(x)
        lines.set_ydata(y)

        ax.relim()
        ax.autoscale_view(True, True, True)

        fig.canvas.draw()
        plt.pause(0.001)
