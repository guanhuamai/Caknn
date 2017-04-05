from Painful.PainfulMaster import data_service

"""
Master script, run this script with configuration file in the machine that
you hope to gather information.
Generate the configuration file using "GenParameters.py" script.
Easy enough to start the service...

"""

# input: local ip, listening port, configuration file
data_service('10.214.147.34', 54321, "painful.configlist")
