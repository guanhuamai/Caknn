from Painful.PainfulUtil import get_configuration
from Painful.PainfulUtil import save_data

from numpy import random
import time
import os


"""
Run this script on the slave machine.  This will take an thread from the slave machine
and use the thread to finish your job until no more configuration sent by the master.

When there is no more configuration, the master will return a "failure" for now, so
you can catch that string and then break from loop. I believe that this is a defect of my
Painful project.

get_configuration, is "failure" ? break the loop : do the job, then get next configuration

"""

master_addr = "10.214.147.34", 54321

config_str = get_configuration(master_addr).decode("utf-8")

while config_str != "failure":

    serie_str = '%d' % (round(time.time() * 1000) + random.random_integers(0, 100))

    config_str = serie_str + " " + config_str.strip('\n')

    cmmd = "../cmake-build-debug/CaknnSR" + " " + config_str

    os.system(cmmd)

    with open(serie_str + ".caknnsr.log", "r") as f:
        lines = f.readlines()
    run_res = reduce(lambda x, y: x + y, lines)

    save_data(master_addr, "caknn.result", run_res)

    config_str = get_configuration(master_addr).decode("utf-8")
