#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import argparse
import re
import math
import scipy.spatial.transform


def test_euler():
    parser = argparse.ArgumentParser(description='Converts a set of Euler angles')
    parser.add_argument('angles', nargs=3, type=float, help='the 3 Euler angles to convert')
    parser.add_argument('-d', '--degrees', action='store_true', help='values are in degrees')
    parser.add_argument('-v', '--verbose', action='store_true', help='write out more information whilst processing')
    args = parser.parse_args()

    if args.verbose:
        pretty_print_sys_argv(sys.argv)
        pretty_print_argparse_args(args)

    euler_angles = ['zxz', 'xyx', 'yzy', 'zyz', 'xzx', 'yxy'] # proper Euler angles
    cardan_angles = ['xyz', 'yzx', 'zxy', 'xzy', 'zyx', 'yxz'] # aka Tait–Bryan angles

    # set{‘X’, ‘Y’, ‘Z’} for intrinsic (rotating coordinate system) rotations,
    # set{‘x’, ‘y’, ‘z’} for extrinsic (static coordinate system) rotations

    # static
    for seq in cardan_angles + euler_angles:
        seq2 = seq.lower()
        rot = scipy.spatial.transform.Rotation.from_euler(seq2, args.angles, degrees=args.degrees)
        rot_quat = rot.as_quat() # quaternions are [x, y, z, w]
        rot_vec = rot.as_rotvec() # the manitude is the angle and the axis is the direction
        angle = Magnitude3x1(rot_vec)
        axis = Normalise3x1(rot_vec)
        if args.degrees: angle = angle * 180.0 / math.pi
        print('extrinsic/static %s Euler %+.3f %+.3f %+.3f Angle-Axis %+.3f %+.3f %+.3f %+.3f Quaternion %+.3f %+.3f %+.3f %+.3f' %
              (seq2, args.angles[0], args.angles[1], args.angles[2],
               angle, axis[0], axis[1], axis[2],
               rot_quat[3], rot_quat[0], rot_quat[1], rot_quat[2]))

    # dynamic
    for seq in cardan_angles + euler_angles:
        seq2 = seq.upper()
        rot = scipy.spatial.transform.Rotation.from_euler(seq2, args.angles, degrees=args.degrees)
        rot_quat = rot.as_quat() # quaternions are [x, y, z, w]
        rot_vec = rot.as_rotvec() # the manitude is the angle and the axis is the direction
        angle = Magnitude3x1(rot_vec)
        axis = Normalise3x1(rot_vec)
        if args.degrees: angle = angle * 180.0 / math.pi
        print('intrinsic/rotate %s Euler %+.3f %+.3f %+.3f Angle-Axis %+.3f %+.3f %+.3f %+.3f Quaternion %+.3f %+.3f %+.3f %+.3f' %
              (seq2, args.angles[0], args.angles[1], args.angles[2],
               angle, axis[0], axis[1], axis[2],
               rot_quat[3], rot_quat[0], rot_quat[1], rot_quat[2]))


# add two vectors
def Add3x1(a, b):
    c = [0, 0, 0]
    c[0] = a[0] + b[0]
    c[1] = a[1] + b[1]
    c[2] = a[2] + b[2]
    return c

# subtract two vectors
def Sub3x1(a, b):
    c = [0, 0, 0]
    c[0] = a[0] - b[0]
    c[1] = a[1] - b[1]
    c[2] = a[2] - b[2]
    return c

# multiply two vectors element by element
def Mul3x1(a, b):
    c = [0, 0, 0]
    c[0] = a[0] * b[0]
    c[1] = a[1] * b[1]
    c[2] = a[2] * b[2]
    return c

# Divide two vectors element by element
def Div3x1(a, b):
    c = [0, 0, 0]
    c[0] = a[0] / b[0]
    c[1] = a[1] / b[1]
    c[2] = a[2] / b[2]
    return c

# calculate cross product (vector product)
def CrossProduct3x1(a, b):
    c = [0, 0, 0]
    c[0] = a[1] * b[2] - a[2] * b[1]
    c[1] = a[2] * b[0] - a[0] * b[2]
    c[2] = a[0] * b[1] - a[1] * b[0]
    return c

# calculate dot product (scalar product)
def DotProduct3x1(a, b):

    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]

# calculate length of vector
def Magnitude3x1(a):

    return math.sqrt((a[0]*a[0]) + (a[1]*a[1]) + (a[2]*a[2]))

# reverse a vector
def Reverse3x1(a):

    return [-a[0], -a[1], -a[2]]

def Normalise3x1(a):
    s = Magnitude3x1(a)
    c = [0, 0, 0]
    c[0] = a[0] / s
    c[1] = a[1] / s
    c[2] = a[2] / s
    return c

# calculate length of vector
def Magnitude4x1(a):

    return math.sqrt((a[0]*a[0]) + (a[1]*a[1]) + (a[2]*a[2]) + (a[3]*a[3]))

def Normalise4x1(a):
    s = Magnitude4x1(a)
    c = [0, 0, 0, 0]
    c[0] = a[0] / s
    c[1] = a[1] / s
    c[2] = a[2] / s
    c[3] = a[3] / s
    return c

def QuaternionFromAxisAngle(axis, angle):

    v = Normalise3x1(axis)

    sin_a = math.sin( angle / 2 )
    cos_a = math.cos( angle / 2 )

    q = [0, 0, 0, 0]
    q[1]    = v[0] * sin_a
    q[2]    = v[1] * sin_a
    q[3]    = v[2] * sin_a
    q[0]    = cos_a

    return q

def QuaternionGetAngle(q):

    if (q[0] <= -1):
        return 0 # 2 * pi
    if (q[0] >= 1):
        return 0 # 2 * 0

    return (2*math.acos(q[0]))


def QuaternionGetAxis(q):

    v = [0, 0, 0]
    v[0] = q[1]
    v[1] = q[2]
    v[2] = q[3]
    s = Magnitude3x1(v)
    if (s <= 0): # special case for zero rotation - set arbitrary axis
        v = [1, 0, 0]
    else:
        v[0] = v[0] / s
        v[1] = v[1] / s
        v[2] = v[2] / s
    return v

def GetRotationTo(v0, v1):
    # Based on Stan Melax's article in Game Programming Gems
    #vector a = crossproduct(v1, v2)
    #q.xyz = a
    #q.w = sqrt((v1.Length ^ 2) * (v2.Length ^ 2)) + dotproduct(v1, v2)

    v0 = Normalise3x1(v0)
    v1 = Normalise3x1(v1)

    d = DotProduct3x1(v0, v1)
    # If dot == 1, vectors are the same
    if (d >= 1.0):
        return [1, 0, 0, 0] # identity quaternion

    if (d < (1e-7 - 1.0)):
        # Generate an axis
        axis = CrossProduct3x1([1, 0, 0], v0)
        if (Magnitude3x1(axis) < 1e-6 and Magnitude3x1(axis) > -1e-6): # pick another if colinear
            axis = CrossProduct3x1([0, 1, 0], v0)
        axis= Normalise3x1(axis)
        q = QuaternionFromAxisAngle(axis, math.pi)

    else:

        s = math.sqrt( (1+d)*2 )
        invs = 1 / s

        c = CrossProduct3x1(v0, v1)

        q = [0, 0, 0, 0]
        q[1] = c[0] * invs
        q[2] = c[1] * invs
        q[3] = c[2] * invs
        q[0] = s * 0.5
        q = Normalise4x1(q)

    return q

def QuaternionVectorRotate(q, v):

    QwQx = q[0] * q[1]
    QwQy = q[0] * q[2]
    QwQz = q[0] * q[3]
    QxQy = q[1] * q[2]
    QxQz = q[1] * q[3]
    QyQz = q[2] * q[3]

    rx = 2* (v[1] * (-QwQz + QxQy) + v[2] *( QwQy + QxQz))
    ry = 2* (v[0] * ( QwQz + QxQy) + v[2] *(-QwQx + QyQz))
    rz = 2* (v[0] * (-QwQy + QxQz) + v[1] *( QwQx + QyQz))

    QwQw = q[0] * q[0]
    QxQx = q[1] * q[1]
    QyQy = q[2] * q[2]
    QzQz = q[3] * q[3]

    rx += v[0] * (QwQw + QxQx - QyQy - QzQz)
    ry += v[1] * (QwQw - QxQx + QyQy - QzQz)
    rz += v[2] * (QwQw - QxQx - QyQy + QzQz)

    return [rx, ry, rz]

def QuaternionQuaternionMultiply(q1, q2):
    return  [ q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2] - q1[3]*q2[3],
              q1[0]*q2[1] + q1[1]*q2[0] + q1[2]*q2[3] - q1[3]*q2[2],
              q1[0]*q2[2] + q1[2]*q2[0] + q1[3]*q2[1] - q1[1]*q2[3],
              q1[0]*q2[3] + q1[3]*q2[0] + q1[1]*q2[2] - q1[2]*q2[1]]

class Matrix:
    def __init__(self,
                 r1c1=0.0, r1c2=0.0, r1c3=0.0,
                 r2c1=0.0, r2c2=0.0, r2c3=0.0,
                 r3c1=0.0, r3c2=0.0, r3c3=0.0):
        self.e11 = r1c1
        self.e12 = r1c2
        self.e13 = r1c3
        self.e21 = r2c1
        self.e22 = r2c2
        self.e23 = r2c3
        self.e31 = r3c1
        self.e32 = r3c2
        self.e33 = r3c3

class Quaternion:
    def __init__(self, nn=1.0, xx=0.0, yy=0.0, zz=0.0):
        self.n = nn
        self.x = xx
        self.y = yy
        self.z = zz

def QuaternionFromMatrix(R):
    # quaternion is [w, x, y, z]
    # matrix is:
    # R.e11 R.e12 R.e13
    # R.e21 R.e22 R.e23
    # R.e31 R.e32 R.e33
    q = [0,0,0,0]
    tr = R.e11 + R.e22 + R.e33
    if (tr >= 0):
        s = math.sqrt (tr + 1)
        q[0] = 0.5 * s
        s = 0.5 * (1.0/s)
        q[1] = (R.e32 - R.e23) * s
        q[2] = (R.e13 - R.e31) * s
        q[3] = (R.e21 - R.e12) * s
        return q
    else:
        # find the largest diagonal element and jump to the appropriate case
        if (R.e22 > R.e11):
            if (R.e33 > R.e22):
                s = math.sqrt((R.e33 - (R.e11 + R.e22)) + 1)
                q[3] = 0.5 * s
                s = 0.5 * (1.0/s)
                q[1] = (R.e31 + R.e13) * s
                q[2] = (R.e23 + R.e32) * s
                q[0] = (R.e21 - R.e12) * s
                return q
            s = math.sqrt((R.e22 - (R.e33 + R.e11)) + 1)
            q[2] = 0.5 * s
            s = 0.5 * (1.0/s)
            q[3] = (R.e23 + R.e32) * s
            q[1] = (R.e12 + R.e21) * s
            q[0] = (R.e13 - R.e31) * s
            return q
        if (R.e33 > R.e11):
            s = math.sqrt((R.e33 - (R.e11 + R.e22)) + 1)
            q[3] = 0.5 * s
            s = 0.5 * (1.0/s)
            q[1] = (R.e31 + R.e13) * s
            q[2] = (R.e23 + R.e32) * s
            q[0] = (R.e21 - R.e12) * s
            return q
        s = math.sqrt((R.e11 - (R.e22 + R.e33)) + 1)
        q[1] = 0.5 * s
        s = 0.5 * (1.0/s)
        q[2] = (R.e12 + R.e21) * s
        q[3] = (R.e31 + R.e13) * s
        q[0] = (R.e32 - R.e23) * s
        return q

def MatrixFromQuaternion(R):
    # quaternion is R[w, x, y, z] (externally)
    # quaternion is q.n q.x qy q.z (internally)
    # matrix is:
    # m.e11 m.e12 m.e13
    # m.e21 m.e22 m.e23
    # m.e31 m.e32 m.e33
    m = Matrix()
    q = Quaternion(R[0], R[1], R[2], R[3])
    qq1 = 2*q.x*q.x
    qq2 = 2*q.y*q.y
    qq3 = 2*q.z*q.z
    m.e11 = 1 - qq2 - qq3
    m.e12 = 2*(q.x*q.y - q.n*q.z)
    m.e13 = 2*(q.x*q.z + q.n*q.y)
    m.e21 = 2*(q.x*q.y + q.n*q.z)
    m.e22 = 1 - qq1 - qq3
    m.e23 = 2*(q.y*q.z - q.n*q.x)
    m.e31 = 2*(q.x*q.z - q.n*q.y)
    m.e32 = 2*(q.y*q.z + q.n*q.x)
    m.e33 = 1 - qq1 - qq2
    return m

def MatrixMul(m1, m2):
    m = Matrix(m1.e11*m2.e11 + m1.e12*m2.e21 + m1.e13*m2.e31,
               m1.e11*m2.e12 + m1.e12*m2.e22 + m1.e13*m2.e32,
               m1.e11*m2.e13 + m1.e12*m2.e23 + m1.e13*m2.e33,
               m1.e21*m2.e11 + m1.e22*m2.e21 + m1.e23*m2.e31,
               m1.e21*m2.e12 + m1.e22*m2.e22 + m1.e23*m2.e32,
               m1.e21*m2.e13 + m1.e22*m2.e23 + m1.e23*m2.e33,
               m1.e31*m2.e11 + m1.e32*m2.e21 + m1.e33*m2.e31,
               m1.e31*m2.e12 + m1.e32*m2.e22 + m1.e33*m2.e32,
               m1.e31*m2.e13 + m1.e32*m2.e23 + m1.e33*m2.e33)
    return m

def MatrixTrans(m):
    mt = Matrix(m.e11,m.e21,m.e31,m.e12,m.e22,m.e32,m.e13,m.e23,m.e33)
    return mt


def preflight_read_file(filename, verbose):
    if verbose: print('preflight_read_file: "%s"' % (filename))
    if not os.path.exists(filename):
        print("Error: \"%s\" not found" % (filename))
        sys.exit(1)
    if not os.path.isfile(filename):
        print("Error: \"%s\" not a file" % (filename))
        sys.exit(1)

def preflight_write_file(filename, force, verbose):
    if verbose: print('preflight_write_file: "%s"' % (filename))
    if os.path.exists(filename) and not os.path.isfile(filename):
        print("Error: \"%s\" exists and is not a file" % (filename))
        sys.exit(1)
    if os.path.exists(filename) and not force:
        print("Error: \"%s\" exists. Use --force to overwrite" % (filename))
        sys.exit(1)

def preflight_read_folder(folder, verbose):
    if verbose: print('preflight_read_folder: "%s"' % (folder))
    if not os.path.exists(folder):
        print("Error: \"%s\" not found" % (folder))
        sys.exit(1)
    if not os.path.isdir(folder):
        print("Error: \"%s\" not a folder" % (folder))
        sys.exit(1)

def preflight_write_folder(folder, verbose):
    if verbose: print('preflight_write_folder: "%s"' % (folder))
    if os.path.exists(folder):
        if not os.path.isdir(folder):
            print("Error: \"%s\" exists and is not a folder" % (folder))
            sys.exit(1)
    else:
        try:
            os.makedirs(folder, exist_ok = True)
        except OSError as error:
            print(error)
            print('Directory "%s" can not be created' % folder)
            sys.exit(1)

def is_a_number(string):
    """checks to see whether a string is a valid number"""
    if re.match(r'^([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?$', string.strip()) == None:
        return False
    return True

def pretty_print_sys_argv(sys_argv):
    quoted_sys_argv = quoted_if_necessary(sys_argv)
    print((" ".join(quoted_sys_argv)))

def pretty_print_argparse_args(argparse_args):
    for arg in vars(argparse_args):
        print(("%s: %s" % (arg, getattr(argparse_args, arg))))

def quoted_if_necessary(input_list):
    output_list = []
    for item in input_list:
        if re.search(r"[^a-zA-Z0-9_.-]", item): # note inside [] backslash quoting does not work so a minus sign to match must occur last
            item = "\"" + item + "\""
        output_list.append(item)
    return output_list
# program starts here

if __name__ == '__main__':
    test_euler()
