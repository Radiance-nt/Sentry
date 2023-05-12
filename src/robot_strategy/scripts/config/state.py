from enum import Enum


class State(Enum):
    WAIT = 1
    NAV_FORWARD = 2
    NAV_HOME = 3
    OUT_OF_BULLET = 6
