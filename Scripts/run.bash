#!/usr/bin/env bash
for((i=1; i<=3; i++)); do
	{
		python PainfulSlaveDo.py
	}&
done
