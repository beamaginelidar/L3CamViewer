import numpy as np
import open3d as o3d


#TODO: Change the filename
filename = "../sample_data/115550076.bin"
file = open(filename, "rb")

#read number of points
number_points = int.from_bytes(file.read(4), byteorder='little', signed=False)

print("num points " + str(number_points))

pcl = o3d.geometry.PointCloud()

points = []
colors = []

#Read all the points and show the data
for i in range(number_points):   
    x = int.from_bytes(file.read(4), byteorder='little', signed=True)
    y = int.from_bytes(file.read(4), byteorder='little', signed=True)
    z = int.from_bytes(file.read(4), byteorder='little', signed=True)
    intensity = int.from_bytes(file.read(4), byteorder='little', signed=True)
    rgb = int.from_bytes(file.read(4), byteorder='little', signed=True)
    b = rgb & 0xFF  
    g = (rgb >> 8) & 0xFF  
    r = (rgb >> 16) & 0xFF 
    points.append([x,y,z])
    colors.append([r/255,g/255,b/255])

         
file.close()

pcl.points = o3d.utility.Vector3dVector(np.array(points))
pcl.colors = o3d.utility.Vector3dVector(np.array(colors))
o3d.visualization.draw_geometries([pcl])
