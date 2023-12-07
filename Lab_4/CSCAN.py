import sys


def find_closest_request(head, requests):
    closest_request = None
    shortest_distance = sys.maxsize

    for request in requests:
        distance = abs(head - request)

        if head <= request:
            if distance < shortest_distance:
                closest_request = request
                shortest_distance = distance

    return shortest_distance, closest_request


def cscan(head, requests, upper_bound):
    queue = requests[:]

    total_head_movement = 0

    while queue:
        distance, request = find_closest_request(head, queue)

        if request is None:
            total_head_movement += abs(head - upper_bound) + upper_bound
            head = 0
        else:
            total_head_movement += distance
            head = request
            queue.remove(request)

    print("C-SCAN: ", total_head_movement)
