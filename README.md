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
- 3x Lilygo ESP32 Lora 32 (SX1276)
- 1x Mobile Phone (Based in factory, acting as wifi-hotspot)

# <h3>Programming Language Used</h3>
This project is developed using Arduino (C++)

# <h3>Assumptions</h3>
- Customers informed of the correct temperature for the storage of the products.
- WiFi Access Point (AP) at factory is always on.
- There will be less than 16 sensor nodes at the customer's compound (see report for further details)
- There will be less than 255 sensor nodes at the factory compound (see report for further details)

# <h3>Flow explanation and justification</h3>
   - The factory itself is a LoRa-MESH network, and the entire network currently consists of 3 Lilygo ESP32s and 1 mobile phone.
       - 1 Lilygo ESP32 is based inside the truck.
       - 1 Lilygo ESP32 is carried by the delivery man.
       - 1 Lilygo ESP32 acts as a central lora gateway.
       - 1 Mobile Phone for the purpose of acting as a WiFi hotspot.
   -  At this stage, nothing significant is happening as of yet. Delivery man will carry 1 Lilygo ESP32 when fulfilling his delivery schedule.
   - Once the truck is at a distributor parking lot:
     - Temperature from the truck is recorded and stored inside of the truck's Lilygo ESP32.
     - The delivery man will take his assigned Lilygo ESP32 and disembark from the truck to proceed to the distributor's freezer to carry out his delivery.
   - His Lilygo ESP32 will act as a middle man, connecting the truck's Lilygo ESP32 and the distributor's freezer.<br>
   - When the freezer's lilygo ESP32 and the delivery man's ESP32 is in range, they will then transmit data from the freezer to the delivery man's ESP32 through a WiFi Mesh.
   - When the driver goes back to the truck, the driver's ESP32 will transmit data to the esp32 in the truck (WiFi mesh)
   - This process repeats multiple times, with the delivery man's ability to go to multiple distributor center to complete his delivery. The driver and truck's ESP32 will build up the sensor data based on the multiple locations the driver has been to.
   - <b>Justification:</b> This enables seamless data communication between the delivery man, distributor's freezer, and truck.

   - Back at the factory, both the delivery man and the truck's ESP32 will transmit to a central lora gateway bridge (LoRa Mesh).
   - From the lora gateway, the data received will be uploaded to a HTTPS API endpoint (WiFi P2P).

       <b>Justification:</b> All these processes are done to protect the manufacturer, such that if the distributor decides to exchange goods due to food spoilage, the manufacturer has some temperature data to track what may be the cause of the issue. As the temperature data are being tracked consistently, evidence can be provided to support the factory in the case of a dispute.

# <h3>System Architectural Diagram</h3>
![image](https://github.com/brianlee51/CSC2106_Team06/assets/115568954/df998095-48c9-400d-bd2e-b32f2bf0f709)



# <h3>Workload Distribution</h3>
The following is the initial workload plan given to each of the respective members of the group. 
However, it is good to note that members could help out in the project outside of their given scope. 
| Working directory  | Done By |
| ------------- | ------------- |
| LoRa Mesh  | Ray Lim Le Hao  |
| WiFi Mesh  | Brian Lee Zheng Jie  |
| WiFi P2P  | Joyce Teng Min Li  |
| SD Card Logging  | Jovan Lee Hong Xi  |
| Temperature Readings  | Tan Jia Wen  |

<details>
  <summary><h3>Initial Project Plans</h3></summary>
  <h4>Stage 1</h4>
  The initial idea of the project was simplified, such that it only requires the usage of M5Stick and NB-IOT for the monitoring of data, such that whenever a truck reaches any distribution center, the truck M5Stick would connect to the internet and then upload the data onto the cloud.<br>
  
  However, this did not come into fruition due to the following factors:<br>
  - The implementation of the project is too simple.
  - The usage of NB-IOT would significantly raise the cost of the product, and we were presented with the fact that consumers will not want to due with this cost.

  <h4>Stage 2</h4>
  Subsequently, the complexity of the project is enhanced, based on this software architecture diagram:
  
  ![Screenshot 2024-02-22 185459](https://github.com/brianlee51/CSC2106_Team06/assets/41094581/ef7473ca-f8d0-4eeb-a1ba-c476e8a60736)

  To better understand the diagram, the updated project plan now includes the use of M5Stick at the factory and Raspberry Pi Pico in the truck as a temperature monitor and data storage for the truck.<br>
  The distribution centres acts as an offline LORA-mesh node, where the temperature data is uploaded back to the truck.<br>
  Once the truck reaches back at the factory, all the data collected would then be uploaded to the cloud via the use of internet.<br>

  However, this plan also did not come into fruition due to the following factors:<br>
  - This way of doing doesn't factor in what happens after the distribution center.
  - Real time monitoring of data is impossible to track without the use of internet.

   <h4>Stage 3</h4>
   
  ![image](https://github.com/brianlee51/CSC2106_Team06/assets/41094581/681501f0-6a99-4751-8faa-f4d52b40c35f)

   As a recap, the finalised at this stage used to be:

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
- Once at the distributor's freezer, all the temperature data collected from the last completed delivery would first be uploaded to the company mobile device, then at close proximity to the truck transfer the data back to the truck's Lilygo<br>
   - The distribution centers form a bluetooth mesh.
   - All data transfer and passing will be done through Bluetooth. <br>
  <b>Justification:</b> The reason for collecting all the temperature data collected from the last completed delivery is so to protect the factory. If the customer had chosen to request for refund multiple times, past records of the customer's freezer temperature data would be useful to determine if the fault lies with the customer's faulty freezer or the temperature being set wrongly.

However, this idea was eventually dropped from the scope, after significant coding and testing of the product. This was due to the fact that:

   - Lora Mesh was not supported in the hardware that we intend to use, in this case, it was the Lilygo ESP32 (SX1280).
</details>

# <h3>References</h3>
