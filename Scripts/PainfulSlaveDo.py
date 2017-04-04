from Painful.PainfulUtil import get_configuration
from Painful.PainfulUtil import save_data

from numpy import random
import time
import os

master_addr = "10.214.147.34", 54321

config_str = get_configuration(master_addr).decode("utf-8")

serie_str = str(round(time.time() * 1000) + random.random_integers(0, 100))

stdin_res = os.popen("../CaknnSR" + " " + config_str + " " + serie_str).read()

with open(serie_str + ".res", "r") as f:
    lines = f.readlines()
run_res = reduce(lambda x, y: x + y, lines)

save_data(master_addr, "run.result", run_res)
