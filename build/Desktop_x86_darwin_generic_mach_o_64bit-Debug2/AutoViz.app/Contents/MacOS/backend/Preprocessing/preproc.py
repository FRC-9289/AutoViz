import pandas as pd # type: ignore
import numpy as np # type: ignore
import sys
import subprocess
import os
import json
from math import radians

try:
    projectName = sys.argv[1]

    with open("../projects.json","r") as f:
        projects = json.load(f)
        width = projects[projectName]['Robot']['Width']
        height = projects[projectName]['Robot']['Length']
        module_type= projects[projectName]['Robot']['module-type']
    
    with open("../moduledata.json", "r") as f:
        module_data = json.load(f)
        max_module_speed= module_data[module_type]['max_velocity_mps']

    df=pd.read_csv(f"../{projectName}.csv")
except Exception as e:
    print(f"Error: {e}")
    sys.exit(1)

def remove_duplicates(df) -> pd.DataFrame:
    """
    Remove duplicate rows based on all columns except 'T'.
    If duplicates are found, sum their 'T' values.
    """
    # Ensure 'T' is numeric for summation    
    df = df.reset_index(drop=True)
    row = 0

    while row < len(df) - 1:
        current_ = df.iloc[row]
        forward_ = df.iloc[row + 1]

        # Compare all columns except 'T'
        if (current_.drop(labels='timestamp') == forward_.drop(labels='timestamp')).all():
            # Sum the T values
            df.at[row, 'timestamp'] += df.at[row + 1, 'timestamp']
            # Drop the next row (it's a duplicate)
            df.drop(index=row + 1, inplace=True)
            df.reset_index(drop=True, inplace=True)
            # Don't increment row since the next row may still be a duplicate
        else:
            row += 1
    
    return df

clean_df = remove_duplicates(df)

def getModuleData(width: float, height: float, LB_angle, LF_angle, RB_angle, RF_angle, LB_velocity, LF_velocity, RB_velocity, RF_velocity) -> str:
    result = subprocess.run([
        "./motionmodel",
        str(width), str(height),
        str(radians(LB_angle)), str(radians(LF_angle)), str(radians(RB_angle)), str(radians(RF_angle)),
        str(LB_velocity), str(LF_velocity), str(RB_velocity), str(RF_velocity)
    ], 
    capture_output=True, 
    text=True,
    cwd="../Motion Modelling/build")

    return result.stdout.strip()

for i in range(len(clean_df)):

    LB_angle = clean_df.iloc[i]['LB_angle'] #Fix AtIndex Error
    LF_angle = clean_df.iloc[i]['LF_angle']
    RB_angle = clean_df.iloc[i]['RB_angle']
    RF_angle = clean_df.iloc[i]['RF_angle']

    LB_velocity = clean_df.iloc[i]['LB_velocity']*max_module_speed
    LF_velocity = clean_df.iloc[i]['LF_velocity']*max_module_speed
    RB_velocity = clean_df.iloc[i]['RB_velocity']*max_module_speed
    RF_velocity = clean_df.iloc[i]['RF_velocity']*max_module_speed

    module_data = getModuleData(width, height, LB_angle, LF_angle, RB_angle, RF_angle, LB_velocity, LF_velocity, RB_velocity, RF_velocity)
    
    v_x, v_y, omega = module_data.strip().split()


    v_x = float(v_x)
    v_y = float(v_y)
    omega = float(omega)

    clean_df.loc[i, 'v_x'] = v_x
    clean_df.loc[i, 'v_y'] = v_y
    clean_df.loc[i, 'omega'] = omega
    
clean_df.to_csv(f"../{projectName}.csv", index=False)