"""builds a hamiltonian cycle over the grid; needs rows*cols even (bipartite parity)"""


def build_hamiltonian_cycle(cols, rows):
    if (cols * rows) % 2 != 0:
        raise ValueError('a Hamiltonian cycle needs an even number of cells')

    # col 0 is the return lane, top to bottom
    cycle = [(0, row) for row in range(rows)]

    # other cols snake up/down, skip row 0 to keep it free for the close
    for col in range(1, cols):
        if col % 2 == 1:
            cycle.extend((col, row) for row in range(rows - 1, 0, -1))
        else:
            cycle.extend((col, row) for row in range(1, rows))

    # close loop along row 0 back to start
    cycle.extend((col, 0) for col in range(cols - 1, 0, -1))
    return cycle
