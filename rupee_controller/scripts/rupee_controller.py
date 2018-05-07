#! /usr/bin/env python
#ROS libraries
import roslib; roslib.load_manifest('kinova_demo')
import rospy
import actionlib
#Python libraries
import sys
import numpy as np
from random import uniform
import math
#ROS messages
import kinova_msgs.msg
import std_msgs.msg
import geometry_msgs.msg
from kinova_msgs.msg import ArmPoseAction
from rupee_msgs.msg import object_pos
from rupee_msgs.msg import Setup
from std_msgs.msg import String
from std_msgs.msg import Int8
from std_msgs.msg import Bool
from geometry_msgs.msg import WrenchStamped
from geometry_msgs.msg import PoseStamped


""" Global variables """
#arm_joint_number = 0
#finger_number = 0
#finger_maxDist = 18.9/2/1000  # max distance for one finger
#currentCartesianCommand = [0.212322831154, -0.257197618484, 0.509646713734, 1.63771402836, 1.11316478252, 0.134094119072] # default home in unit mq
#ori2 =[0.962,  0.267,  -0.000,  0.067]
#done = True
prefix = 'j2n6s300_'
finger_maxTurn = 6800  # max thread rotation for one finger
home_pos  = [0.212322592735,-0.256329715252,0.5066832304]
home_ori  = [0.64524179697,0.319341748953,0.42331302166,0.549990773201]
home_pose = [home_pos,home_ori]
workspace_constant = [0.0,-0.60,0.1]
object_location = [0,0,0]
offset = 0.09
move = False
loop = True
exercise_type = 1
exercise_difficulty = 1
orientation = [0.962,  0.267,  -0.000,  0.067]#[0.999,0.01,-0.02,0.005]
temp_location = [0,0,0]



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
    client.send_goal(goal)

    if client.wait_for_result(rospy.Duration(15.0)):
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

def fingerAction(action):
    if(action == "close"):
	    finger_val = [75,75,75]
    elif(action == "open"):
	    finger_val = [0,0,0]
    finger_turn_command = [x/100.0 * finger_maxTurn for x in finger_val]
    finger_turn_ = finger_turn_command
    try:
        positions_temp1 = [max(0.0, n) for n in finger_turn_]
        positions_temp2 = [min(n, finger_maxTurn) for n in positions_temp1]
        positions = [float(n) for n in positions_temp2]
        result = gripper_client(positions)
    except rospy.ROSInterruptException:
            print('program interrupted before completion')

def moveArm(gripper_pos,ori,finger_status):
    success = cartesian_pose_client(gripper_pos,ori)
    while not success:
        pass
    if not finger_status == "":
        fingerAction(finger_status)
    #JACOFeedbackPub(False)

def exerciseRun(exercise,difficulty):
    print (exercise)
    if exercise == 1:
        exerciseOne(difficulty)
    elif exercise == 2:
        exerciseTwo(difficulty)
    elif exercise == 3:
        exerciseThree(difficulty)
    elif exercise == 4:
        exerciseFour(difficulty)
    elif exercise == 5:
        exerciseFive(difficulty)
    else:
        print("Exercise type must be a number between 1-5.")

def exerciseOne(difficulty):
    rand_pos = randomCoordinatesOne(difficulty)
    moveArm(object_location,orientation,"close")
    moveArm(rand_pos,orientation,"open")
    print('Random position is :', rand_pos )
    moveArm(home_pose[0],home_pose[1],"")
def exerciseTwo(difficulty):
    rand_pos = randomCoordinatesTwo(difficulty)
    moveArm(object_location,orientation,"close")
    moveArm(rand_pos,orientation,"")
    print('Random position is :', rand_pos )
    if (object_location[2] >0.05):
        moveArm(home_pose[0],home_pose[1],"open")
def exerciseThree(difficulty):
    pass
def exerciseFour(difficulty):
    pass
def exerciseFive(difficulty):
    pass

def randomCoordinatesOne(difficulty):
    workspace =[difficulty*0.14,difficulty*0.11]
    if (difficulty == 3):
        workspace[1] = 2*0.11
    random_x = workspace_constant[0] + uniform(-workspace[0],workspace[0])
    random_y = workspace_constant[1] + uniform(0.0,workspace[1])
    random_z = workspace_constant[2]
    random_pos  = [random_x,random_y,random_z]
    if difficulty > 1:
        sub_workspace = [workspace_constant[0]+(difficulty-1)*0.14,workspace_constant[1]+(difficulty-1)*0.11]
        if (object_location[0]-0.05 <= random_x <= object_location[0] + 0.05 and object_location[1]-0.05 <= random_y <= object_location[1] + 0.05
        or  -sub_workspace[0] <= random_x <= sub_workspace[0] and workspace_constant[1] <= random_y <= sub_workspace[1]):
            return (randomCoordinatesOne(difficulty))
    else:
        if(object_location[0]-0.05 <= random_x <= object_location[0] + 0.05 and object_location[1]-0.05 <= random_y <= object_location[1] + 0.05):
            return (randomCoordinatesOne(difficulty))
    return random_pos

def randomCoordinatesTwo(difficulty):
    global temp_location
    workspace =[difficulty*0.14,difficulty*0.11,difficulty*0.11]
    if (difficulty == 3):
        workspace[1] = 2*0.11
    random_x = workspace_constant[0] + uniform(-workspace[0],workspace[0])
    random_y = workspace_constant[1] + uniform(0.0,workspace[1])
    random_z = workspace_constant[2] + uniform(0.0,workspace[2])
    random_pos  = [random_x,random_y,random_z]
    if difficulty > 1:
        sub_workspace = [workspace_constant[0]+(difficulty-1)*0.14,workspace_constant[1]+(difficulty-1)*0.11]
        if (temp_location[0]-0.05 <= random_x <= temp_location[0] + 0.05 and temp_location[1]-0.05 <= random_y <= temp_location[1] + 0.05 and temp_location[2]-0.05 <= random_z <= temp_location[2] + 0.05
        or  -sub_workspace[0] <= random_x <= sub_workspace[0] and workspace_constant[1] <= random_y <= sub_workspace[1]):
            return (randomCoordinatesTwo(difficulty))
    else:
        if (temp_location[0]-0.05 <= random_x <= temp_location[0] + 0.05 and temp_location[1]-0.05 <= random_y <= temp_location[1] + 0.05 and temp_location[2]-0.05 <= random_z <= temp_location[2] + 0.05):
            return (randomCoordinatesTwo(difficulty))
    temp_location = random_pos
    return random_pos

def randomCoordinatesThree(difficulty):
    random_x = workspace_constant[0] + uniform(0,1)
    random_y = workspace_constant[1] + uniform(0,1)
    random_z = 0
    random_pos  = [random_x,random_y,random_z]
    return random_pos

def randomCoordinatesFour(difficulty):
    random_x = workspace_constant[0] + uniform(0,1)
    random_y = workspace_constant[1] + uniform(0,1)
    random_z = 0
    random_pos  = [random_x,random_y,random_z]
    return random_pos

def randomCoordinatesFive(difficulty):
    random_x = workspace_constant[0] + uniform(0,1)
    random_y = workspace_constant[1] + uniform(0,1)
    random_z = 0
    random_pos  = [random_x,random_y,random_z]
    return random_pos


'''
def unitParser( finger_value_):
    finger_turn_command = [x/100.0 * finger_maxTurn for x in finger_value_]
    finger_turn_ = finger_turn_command
    return finger_turn_
'''

'''
def gripperAction(status):
	if status == "close":
	    finger_val = [75,75,75]
	elif status == "open":
	    finger_val = [0,0,0]
	return finger_val
'''

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

def JACOFeedbackPub(done):
    pub = rospy.Publisher('JACO_feedback', Bool, queue_size=10)
    rate = rospy.Rate(10) # 10hz
    rep_done = done
    pub.publish(rep_done)
    rate.sleep()

#rate = rospy.Rate
if __name__ == '__main__':
    rospy.init_node(prefix + 'pose_action_client')
    moveArm(home_pose[0],home_pose[1],"open")
    while not rospy.is_shutdown():
        rospy.Subscriber("JACO_goal", object_pos, handlerCallback)
        if(loop):
            JACOFeedbackPub(True)
            loop = False
        if(move):
            try:
                exerciseRun(exercise_type,exercise_difficulty)
                print('Rep done')
                loop = True
            except rospy.ROSInterruptException:
                print "program interrupted before completion"
        else:
            print('Waiting for handler')
