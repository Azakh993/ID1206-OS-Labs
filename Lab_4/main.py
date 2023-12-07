from CSCAN import cscan
from FCFS import fcfs
from SCAN import scan
from SSTF import sstf


def main():
    requests = [176, 79, 34, 60, 92, 11, 41, 114]
    head = 50

    fcfs(head, requests)
    sstf(head, requests)
    scan(head, requests)
    cscan(head, requests)


if __name__ == "__main__":
    main()
