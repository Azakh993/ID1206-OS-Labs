import sys


def find_closest_request(head, requests):
    closest_request = 0
    shortest_distance = sys.maxsize

    for request in requests:
        distance = abs(head - request)

        if distance < shortest_distance:
            closest_request = request
            shortest_distance = distance

    return shortest_distance, closest_request


def sstf(head, requests):
    requests_copy = requests[:]
    total_head_movement = 0

    while len(requests_copy) > 0:
        distance, request = find_closest_request(head, requests_copy)
        total_head_movement += distance
        head = request
        requests_copy.remove(request)

    print("SSTF: ", total_head_movement)

