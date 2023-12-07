import random
import sys
from CLOOK import clook
from CSCAN import cscan
from FCFS import fcfs
from LOOK import look
from SCAN import scan
from SSTF import sstf


def get_script_argument():
    if len(sys.argv) != 2:
        print("Usage: python your_script.py <head_value>")
        return

    try:
        head = int(sys.argv[1])
    except ValueError:
        print("Invalid head value. Please enter a valid integer.")
        return

    return head


def generate_random_numbers(number_of_random_numbers, upper_bound):
    random_numbers = []
    for i in range(number_of_random_numbers):
        random_number = random.randint(0, upper_bound)
        random_numbers.append(random_number)
    return random_numbers


def main():
    head = get_script_argument()

    number_of_random_numbers = 1000
    upper_bound = 4999
    requests = generate_random_numbers(number_of_random_numbers, upper_bound)

    fcfs(head, requests)
    sstf(head, requests)
    scan(head, requests, upper_bound)
    cscan(head, requests, upper_bound)
    look(head, requests)
    clook(head, requests)


if __name__ == "__main__":
    main()
