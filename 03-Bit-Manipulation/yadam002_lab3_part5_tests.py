# Array of tests to run (in order)
# Each test contains
#   description - 
#   steps - A list of steps to perform, each step can have
#       inputs - A list of tuples for the inputs to apply at that step
#       *time - The time (in ms) to wait before continuing to the next step 
#           and before checking expected values for this step. The time should be a multiple of
#           the period of the system
#       *iterations - The number of clock ticks to wait (periods)
#       expected - The expected value at the end of this step (after the "time" has elapsed.) 
#           If this value is incorrect the test will fail early before completing.
#       * only one of these should be used
#   expected - The expected output (as a list of tuples) at the end of this test
# An example set of tests is shown below. It is important to note that these tests are not "unit tests" in 
# that they are not ran in isolation but in the order shown and the state of the device is not reset or 
# altered in between executions (unless preconditions are used).


tests = [ 

    {'description': 'PINB: 0x00, PIND: 0x00 => PINB: 0x00',
    'steps': [ {'inputs': [('PINB',0x00), ('PIND',0x00)], 'iterations': 1 } ],
    'expected': [('PORTB',0x00)],
    },

    {'description': 'PINB: 0x01, PIND: 0x06 => PINB: 0x05',
    'steps': [ {'inputs': [('PINB',0x01), ('PIND',0x06)], 'iterations': 1 } ],
    'expected': [('PORTB',0x05)],
    },

    {'description': 'PINB: 0x00, PIND: 0x06 => PINB: 0x04',
    'steps': [ {'inputs': [('PINB',0x00), ('PIND',0x06)], 'iterations': 1 } ],
    'expected': [('PORTB',0x04)],
    },

    {'description': 'PINB: 0x01, PIND: 0x4F => PINB: 0x03',
    'steps': [ {'inputs': [('PINB',0x01), ('PIND',0x4F)], 'iterations': 1 } ],
    'expected': [('PORTB',0x03)],
    },

    {'description': 'PINB: 0x00, PIND: 0x4F => PINB: 0x02',
    'steps': [ {'inputs': [('PINB',0x00), ('PIND',0x4F)], 'iterations': 1 } ],
    'expected': [('PORTB',0x02)],
    },

    {'description': 'PINB: 0x00, PIND: 0x23 => PINB: 0x02',
    'steps': [ {'inputs': [('PINB',0x00), ('PIND',0x23)], 'iterations': 1 } ],
    'expected': [('PORTB',0x02)],
    },

    ]


watch = ['main::total', 'enabled', 'disabled','tmpB', 't']

# Optionally you can add a set of "watch" variables these need to be global or static and may need
# to be scoped at the function level (for static variables) if there are naming conflicts. The 
# variables listed here will display everytime you hit (and stop at) a breakpoint
#watch = ['<function>::<static-var>','PORTB']

