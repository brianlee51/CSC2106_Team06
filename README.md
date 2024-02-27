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
- 3x Lilygo ESP32
- 1x Mobile Phone (Delivery Man to distributor's factory)
- 1x Mobile Phone (Based in factory, acting as wifi-hotspot)

# <h3>Programming Language Used</h3>
This project is developed using Arduino (C++) and the steps to run the project is as follows:
- IF ANY, TBC

# <h3>Flow explanation and justification</h3>
- The factory itself is a LoRa-MESH network, consisting 2 Lilygo ESP32s and 1 mobile phone.
   - 1 Lilygo ESP32 (Lora/WiFi) to facilitate the receival of information from the other Lilygo (LoRa), and to upload the data received to the cloud (WiFi)
   - 1 Lilygo ESP32 (Lora/Bluetooth) to facilitate the receival of information from the truck (Bluetooth), and passing the information to the other Lilygo (LoRa)
   - Mobile phone is acting as a WiFi Hotspot connection.
 - When the truck leaves the factory, there will not be any internet connection involved. The passing of information is through the usage of Bluetooth.
 - Once the truck is at a distributor parking lot:
   - Temperature from the truck is recorded and stored inside of the truck's Lilygo ESP32.
   - If abnormalities are detected with the temperature once the truck reaches the distributor, the goods will not be unloaded, but rather, displayed as delivery failed and the truck would be recalled back to the factory.
   - Otherwise, the delivery can proceed as per normal. <br>
     <b>Justification:</b> This is done to protect the factory, such that if there are abnormalities, it would ensure retifications are made. At the same time, the distributor cannot blame the factory for cases of food spoilage, as the temperature data are being tracked consistently, providing evidence to support the factory in the case of a dispute.
- When delivery can be done, the delivery man will carry the company's mobile device to deliver the products to the distributor's freezer.
   - This mobile device will act as a middle man, connecting the truck's Lilygo ESP32 and the distributor's freezer. <br>
     <b>Justification:</b> Considering the truck will be parked at the loading/unloading bay, the range between the truck and the freezer may be too far apart for a bluetooth connection. Furthermore, there was a need to justify why one should enable the bluetooth function on their personal mobile device to aid with this connection. Hence, having the delivery man carry a company mobile device would solve the issue.
- Once at the distributor's freezer, all the temperature data collected from the last completed delivery would be 

# <h3>System Architectural Diagram</h3>
![image](https://github.com/brianlee51/CSC2106_Team06/assets/41094581/8f1cb887-e451-448c-b565-0e3e5fc9dce3)

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
