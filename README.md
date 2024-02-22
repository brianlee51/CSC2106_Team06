# Internet-enabled thermometer cluster for Supply Chain Temperature Monitoring
This README.md files consists of details regarding the aim and production of the project, which includes initial plans that were scrapped and the eventual final idea for development, and is aimed for documentation purposes.

# <h3>Group Members</h3>
This project is developed as part of a school project, ultilising IOT resources. The group consists of:
- Ray Lim Le Hao (2201922)
- Brian Lee Zheng Jie (2200471)
- Jovan Lee Hong Xi (2200913)
- Joyce Teng Min Li (2203242)
- Tan Jia Wen (2203393) 

# <h3>Project Description</h3>
This concept-project aims to propose integrating internet-enabled temperature sensors into distributors' and customers' chillers/freezers to enable real-time monitoring throughout the supply chain.<br><br>
It empowers manufacturers to remotely assess and regulate product storage conditions, addressing issues like inadequate cooling or low coolant gas levels in a reasonable amount of time prior, such that neccessary actions could be taken swiftly when an abnormality is detected.<br><br>
This brings the point of our overall project, which aims to reduce the chances of food spoilage resulting in food wastage, by monitoring the temperature data in real time and consequently, able to have the ability to keep track of abnormal data.

# <h3>Equipment Used</h3>
The following is the list of equipment used to develop the project:
- TBC
- TBC
- TBC

# <h3>Programming Language Used</h3>
This project is developed using Arduino (C++) and the steps to run the project is as follows:
- IF ANY, TBC

# <h3>System Architectural Diagram</h3>
[INSERT PICTURE]

# <h3>Workload Distribution</h3>
The following is the initial workload plan given to each of the respective members of the group. 
However, it is good to note that members could help out in the project outside of their given scope. 
| Working directory  | Done By |
| ------------- | ------------- |
| TBC  | TBC  |
| TBC  | TBC  |
| TBC  | TBC  |
| TBC  | TBC  |
| TBC  | <b>TBC</b>  |

<details>
  <summary><h3>Initial Project Plans</h3></summary>
  <h4>Stage 1</h4>
  The initial idea of the project was simplified, such that it only requires the usage of M5Stick and NB-IOT for the monitoring of data, such that whenever a truck reaches any distribution center, the truck M5Stick would connect to the internet and then upload the data onto the cloud.<br>
  
  However, this did not come into fruition due to the following factors:<br>
  - The implementation of the project is too simplified.
  - The usage of NB-IOT would significantly raise the cost of the product, and we were presented with the fact that consumers will not want to due with this cost.

  <h4>Stage 2</h4>
  Subsequently, the complexity of the project is enhanced, based on this software architecture diagram:
  
  ![Screenshot 2024-02-22 185459](https://github.com/brianlee51/CSC2106_Team06/assets/41094581/ef7473ca-f8d0-4eeb-a1ba-c476e8a60736)

  To better understand the diagram, the updated project plan now includes the use of M5Stick at the factory and Raspberry Pi Pico in the truck as a temperature monitor and data storage for the truck.<br>
  The distribution centres acts as an offline LORA-mesh node, where the temperature data is uploaded back to the truck.<br>
  Once the truck reaches back at the factory, all the data collected would then be uploaded to the cloud via the use of internet.<br>

  However, this plan also did not come into fruition due to the following factors:<br>
  - This way of doing doesn't factor in after the distribution center.
  - Real time monitoring of data is impossible to track without the use of internet.
</details>

# <h3>References</h3>
