"""
Change this script directly to generate configuration parameters.
The configurations in the config file will be allocate to the slaves by "PainfulMaster",
All the configurations will be running in slaves, and finally gathered by the master.

Please check "Painful" for more information..
"""

PROJECT_PATH = ["../", ]
DATA_NAME = ["data/alaska", ]
VILLA_NAME = ["bt.mobj", ]
LOG_FILE = ["caknnsr.log", ]
NUM_LANDMARK = ["1", "20", "40", "80", "100"]
NUM_MOBJ = ["50000", ]
K = ["1", "20", "50", "100", "200"]
SPEED = ["10", "50", "250"]
USE_LC = ["useLC", "notUseLC"]
USE_MAD = ["useMAD", "notUseMAD"]
TRAFFIC_FILE_TYPE = ["BRINKHOFF", "COMPACT"]  # The file type of the traffic input, usually BRINKHOFF.
EXPANSION_TYPE = ["SLE", "ILE_MIN", "ILE_SUM"]
AGGREGATE_TYPE = ["AGG_MIN", "AGG_MAX", "AGG_SUM"]


REPEAT = 50


with open("painful.configlist", 'a') as f:
    for i in range(REPEAT):  # repeat for 50 times
        for k in range(len(NUM_LANDMARK)):
            for j in range(len(AGGREGATE_TYPE)):
                f.write(PROJECT_PATH[0] + " " +
                        DATA_NAME[0] + " " +
                        VILLA_NAME[0] + " " +
                        LOG_FILE[0] + " " +
                        NUM_LANDMARK[k] + " " +
                        NUM_MOBJ[0] + " " +
                        K[2] + " " +
                        SPEED[1] + " " +
                        USE_LC[1] + " " +
                        USE_MAD[0] + " " +
                        TRAFFIC_FILE_TYPE[0] + " " +
                        EXPANSION_TYPE[0] + " " +
                        AGGREGATE_TYPE[j]
                        )
                f.write("\n")
