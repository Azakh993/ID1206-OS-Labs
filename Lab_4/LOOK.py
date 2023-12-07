import sys


def find_closest_request(head, requests, direction):
    closest_request = None
    shortest_distance = sys.maxsize

    for request in requests:
        distance = abs(head - request)

        if (direction == 'L' and head >= request) or (direction == 'R' and head <= request):
            if distance < shortest_distance:
                closest_request = request
                shortest_distance = distance

    return shortest_distance, closest_request


def switch_direction(direction):
    return 'R' if direction == 'L' else 'L'


def look(head, requests):
    requests_copy = requests[:]

    total_head_movement = 0
    direction = 'L'

    while requests_copy:
        distance, request = find_closest_request(head, requests_copy, direction)

        if request is None:
            direction = switch_direction(direction)
        else:
            total_head_movement += distance
            head = request
            requests_copy.remove(request)

    print("LOOK: ", total_head_movement)
