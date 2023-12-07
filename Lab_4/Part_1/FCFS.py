def fcfs(head, requests):
    total_head_movement = 0

    for request in requests:
        distance = (abs(head - request))
        total_head_movement += distance
        head = request

    print("FCFS: ", total_head_movement)

