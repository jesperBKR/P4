#! /usr/bin/env python
"""A helper program to test cartesian goals for the JACO and MICO arms."""

import roslib; roslib.load_manifest('kinova_demo')
import rospy

import sys
import numpy as np

import actionlib
import kinova_msgs.msg
from kinova_msgs.msg import ArmPoseAction
import std_msgs.msg
from rupee_msgs.msg import object_pos
from rupee_msgs.msg import Setup
from std_msgs.msg import String
from std_msgs.msg import Int8
from std_msgs.msg import Bool
import geometry_msgs.msg
from random import uniform
import math
import argparse

""" Global variable """
arm_joint_number = 0
finger_number = 0
prefix = 'j2n6s300_'
finger_maxDist = 18.9/2/1000  # max distance for one finger
finger_maxTurn = 6800  # max thread rotation for one finger
currentCartesianCommand = [0.212322831154, -0.257197618484, 0.509646713734, 1.63771402836, 1.11316478252, 0.134094119072] # default home in unit mq
home_pos  = [0.212322592735,-0.256329715252,0.5066832304]
home_ori  = [0.64524179697,0.319341748953,0.42331302166,0.549990773201]
home_pose = [home_pos,home_ori]
object_location = [0,0,0]
offset = 0.10
move = False
done = False
exercise_type = 1
exercise_difficulty = 1
orientation = [0.962,  0.267,  -0.000,  0.067]#[0.999,0.01,-0.02,0.005]
ori2 =[0.962,  0.267,  -0.000,  0.067]



def cartesian_pose_client(position, orientation):
    """Send a cartesian goal to the action server."""
    action_address = '/' + prefix + 'driver/pose_action/tool_pose'
    client = actionlib.SimpleActionClient(action_address, kinova_msgs.msg.ArmPoseAction)
    client.wait_for_server()

    goal = kinova_msgs.msg.ArmPoseGoal()
    goal.pose.header = std_msgs.msg.Header(frame_id=(prefix + 'link_base'))
    goal.pose.pose.position = geometry_msgs.msg.Point(
        x=position[0], y=position[1], z=position[2])
    goal.pose.pose.orientation = geometry_msgs.msg.Quaternion(
        x=orientation[0], y=orientation[1], z=orientation[2], w=orientation[3])

    # print('goal.pose in client 1: {}'.format(goal.pose.pose)) # debug

    client.send_goal(goal)

    if client.wait_for_result(rospy.Duration(10.0)):
        return client.get_result()
    else:
        client.cancel_all_goals()
        print('the cartesian action timed-out')
        return None

def gripper_client(finger_positions):
    """Send a gripper goal to the action server."""
    action_address = '/' + prefix + 'driver/fingers_action/finger_positions'

    client = actionlib.SimpleActionClient(action_address,
                                          kinova_msgs.msg.SetFingersPositionAction)
    client.wait_for_server()

    goal = kinova_msgs.msg.SetFingersPositionGoal()
    goal.fingers.finger1 = float(finger_positions[0])
    goal.fingers.finger2 = float(finger_positions[1])
    # The MICO arm has only two fingers, but the same action definition is used
    if len(finger_positions) < 3:
        goal.fingers.finger3 = 0.0
    else:
        goal.fingers.finger3 = float(finger_positions[2])
    client.send_goal(goal)
    if client.wait_for_result(rospy.Duration(5.0)):
        return client.get_result()
    else:
        client.cancel_all_goals()
        rospy.WARN('the gripper action timed-out')
        return None
def moveArm(gripper_pos,ori,finger_status):
    gripper_pos[2] = gripper_pos[2] + 0.10
    success = cartesian_pose_client(gripper_pos,ori)
    while not success:
        print("Moving to goal")
    if not finger_status == "":
        fingerAction(finger_status)
'''
def kinova_robotTypeParser(kinova_robotType_):
    """ Argument kinova_robotType """
    global robot_category, robot_category_version, wrist_type, arm_joint_number, robot_mode, finger_number, prefix, finger_maxDist, finger_maxTurn
    robot_category = kinova_robotType_[0]
    robot_category_version = int(kinova_robotType_[1])
    wrist_type = kinova_robotType_[2]
    arm_joint_number = int(kinova_robotType_[3])
    robot_mode = kinova_robotType_[4]
    finger_number = int(kinova_robotType_[5])
    prefix = kinova_robotType_ + "_"
    finger_maxDist = 18.9/2/1000  # max distance for one finger in meter
    finger_maxTurn = 6800  # max thread turn for one finger
'''

def random_coordinates(workspace_limit):
    random_x = object_location[0] + uniform(workspace_limit[0],workspace_limit[1])
    random_y = object_location[1] + uniform(workspace_limit[2],workspace_limit[3])
    random_z = object_location[2] + uniform(workspace_limit[4],workspace_limit[5])
    random_pos  = [random_x,random_y,random_z]
    return random_pos

def determineWorkspace(difficulty,exercise):
    if exercise == 1:
        z = [0,0]
    else:
        z = [0.0,0.7]
    if difficulty == 1:
        workspace = [-difficulty*0.05,difficulty*0.05,-difficulty*0.10,difficulty*0.10,z[0],z[1]]#0.14, 0.787,-0.34,-0.74,0.05,0.7]
    elif difficulty == 2:
        workspace = [0,difficulty*0.10,0,difficulty*0.20,z[0],z[1]]
    elif difficulty == 3:
        workspace = [0,difficulty*0.10,0,difficulty*0.20,z[0],z[1]]
    return workspace
'''
def setWorkspace(difficulty,exercise):
    if exercise == 1:
        workspace = determineWorkspace(difficulty)
    elif exercise == 2:
        workspace = determineWorkspace(difficulty)
    elif exercise == 3:
        workspace = determineWorkspace(difficulty)
    elif exercise == 4:
        workspace = determineWorkspace(difficulty)
    elif exercise == 5:
        workspace = determineWorkspace(difficulty)
    return workspace
'''
def unitParser( finger_value_):
    finger_turn_command = [x/100.0 * finger_maxTurn for x in finger_value_]
    finger_turn_ = finger_turn_command
    return finger_turn_

def fingerAction(action):
    try:
        positions_temp1 = [max(0.0, n) for n in unitParser(gripperAction(action))]
        positions_temp2 = [min(n, finger_maxTurn) for n in positions_temp1]
        positions = [float(n) for n in positions_temp2]
        print('Sending finger position ...')
        result = gripper_client(positions)
        print('Finger position sent!')
    except rospy.ROSInterruptException:
        print('program interrupted before completion')

def gripperAction(status):
	if status == "close":
	    finger_val = [75,75,75]
	elif status == "open":
	    finger_val = [0,0,0]
	return finger_val


def handlerCallback(handler_data):
    global move
    global object_location
    global exercise_difficulty
    global exercise_type
    exercise_difficulty = handler_data.difficulty.data
    exercise_type       = handler_data.exercise.data
    object_location[0]  = handler_data.location.x
    object_location[1]  = handler_data.location.y
    object_location[2]  = handler_data.location.z + offset
    move                = handler_data.move.data

def JACOfeedback():
    pub = rospy.Publisher('JACO_feedback', Bool, queue_size=10)
    rate = rospy.Rate(10) # 10hz
    rep_done = done
    pub.publish(rep_done)
    rate.sleep()

def exerciseRun(exercise,difficulty):
    if (exercise == 1 or 2 or 3 or 4):
        if exercise == 1:
            workspace = determineWorkspace(exercise,difficulty)
            rand_pos = random_coordinates(determineWorkspace(exercise,difficulty))
            moveArm(object_location,orientation,"close")
            moveArm(rand_pos,orientation,"open")
            moveArm(home_pose[0],home_pose[1],"open")
        '''
        elif exercise == 2:
            moveArm(object_location,orientation,"close")
            workspace = setWorkspace(exercise,difficulty)
            rand_pos = random_coordinates(workspace)
            moveArm(rand_pos,orientation,"open")
            moveArm(home_pose[0],home_pose[1],"open")
        elif exercise == 3:
            print("TODO Exercise 3")
            #retreiveObject(object_location)
            #TODO implement skeletal tracking to implement.
            #setWorkspace(exercise,difficulty)
        elif exercise == 4:
            moveArm(object_location,orientation,"close")
            workspace = setWorkspace(exercise,difficulty)
            rand_pos_1 = random_coordinates(workspace)
            rand_pos_2 = random_coordinates(workspace)
            moveArm(rand_pos_1,"")
            moveArm(rand_pos_2,"")
            #TODO stop when patient's hand is close to the EE so he can take object.
        '''
    elif exercise == 5:
        print("Todo chase and touch")
        #TODO chase and touch, add later.
    else:
        print("Exercise type must be a number between 1-5.")

if __name__ == '__main__':
    rospy.init_node(prefix + 'pose_action_client')
    for x in range(0, 10):
        print(random_coordinates(determineWorkspace(1,1)))
    #kinova_robotTypeParser(prefix)
    moveArm(home_pose[0],home_pose[1],"open")
    while not rospy.is_shutdown():
        rospy.Subscriber("JACO_goal", object_pos, handlerCallback)
        if(move):
            try:
                print(object_location)
                print(orientation)
                cartesian_pose_client([0.005,-0.70,0.10],orientation)
                #cartesian_pose_client(home_pos,home_ori)
                #done = False
                #exerciseRun(exercise_type,exercise_difficulty)
                #done = True
                #JACOfeedback()
                print('Rep done')
            except rospy.ROSInterruptException:
                print "program interrupted before completion"
        else:
            print('Waiting for handler')
