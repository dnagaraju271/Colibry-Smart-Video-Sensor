# ASYGN Mission Book — Phase 1 (2026-10-08)

> Searchable text extraction from the user-provided PDF reference. Page labels and extracted text are retained where available; figures and page layout may not be represented. Consult the original PDF for diagrams and exact visual formatting.

---

<PARSED TEXT FOR PAGE: 1 / 12>
                                                    ASYGN
                              MISSION BOOK — PHASE 1
                                             AIDGE CHALLENGE

                                           SMART VIDEO SENSOR




                                                MISSION BOOK

                                                     PHASE 1

                                                    08/10/2026




 Information &nbsp;&nbsp;&nbsp;&nbsp; Content

 Date&nbsp;&nbsp;&nbsp;&nbsp;08/10/2026

 Description&nbsp;&nbsp;&nbsp;&nbsp;This document contains the information concerning the first phase of the
 AIDGE Student Challenge. It is intended for students registered for the topic Person Detection and Counting with the
 ColibryNPU Chip.




ASYGN — AIDGE CHALLENGE — SMART VIDEO SENSOR                                                                            1
<PARSED TEXT FOR PAGE: 2 / 12>
 Contents
 1. Welcome to the AIDGE and ColibryNPU world

 2. Glossary

 3. Prerequisites

 4. Objectives and scoring criteria

 5. Schedule

 6. Support

 7. AIDGE design flow

 8. Advice for Phase 1




ASYGN — AIDGE CHALLENGE — SMART VIDEO SENSOR    2
<PARSED TEXT FOR PAGE: 3 / 12>
 1. Welcome to the AIDGE and ColibryNPU world
 Dear AIDGE Challenge participant,

 Welcome to the world of AIDGE and ColibryNPU! I hope you are doing well and feeling motivated! My name is Adrien
 Vialletelle and I am in charge of Artificial Intelligence activities at ASYGN. I will be your point of contact for the next
 three months.

 First of all, I would like to thank you for taking part in the AIDGE Challenge and for choosing the Smart Video Sensor
 topic proposed by ASYGN! I am looking forward to seeing you use our ColibryNPU chip.

 The ColibryNPU is an AI chip that enables electronic devices to understand their environment with virtually zero
 energy consumption. These electronic devices may be medical (a smart patch detecting cardiac abnormalities),
 consumer products (a smart brick enabling voice interaction with toys), or industrial systems (a smart sensor detecting
 the presence of a person, vehicle, or object). Most importantly, they all have one thing in common: very limited energy
 resources. The ColibryNPU brings AI to these devices without impacting their battery — in other words, it makes them
 smarter without compromising their operating time between recharges or battery replacements!

 The topic you have chosen therefore falls within the field of industrial devices. Your mission is to design a smart sensor
 prototype for people counting. I am proposing this topic because AI-based image processing requires significant
 hardware resources, while also being relatively easy for you to implement and test yourself.

 In this first mission book, you will find all the information concerning your objectives, the challenge schedule, and more
 specifically the first phase.

 Together with my whole team, we wish you and your teammates the best of luck and every success with AIDGE and
 ColibryNPU!

 Adrien




ASYGN — AIDGE CHALLENGE — SMART VIDEO SENSOR                                                                                   3
<PARSED TEXT FOR PAGE: 4 / 12>
 2. Glossary
 Below is a list of definitions to know for this challenge:

 Performance: the model's ability to correctly identify the people present in an image, expressed as a percentage (%).

 Latency: the time required by the system to execute the complete image-processing chain (acquisition, detection by
 inference, then producing the counting result), expressed in milliseconds (ms).

 Power consumption: the average electrical power consumed by the system during operation, expressed in milliwatts
 (mW).

 Framerate (FPS): the number of images the system is able to acquire and process per second, expressed in frames
 per second (FPS).

 3. Prerequisites
 To take part in this challenge under good conditions, you should be able to use the following technologies:

 For Machine Learning:
  • Creating and annotating datasets;
  • Neural-network development tools such as TensorFlow or PyTorch (but for this challenge you will use the AIDGE
    platform!);
  • CNN-type neural networks.

 For embedded computing:
  • C/C++ and Python programming languages;
  • GCC compilation and microcontroller programming, and GDB debugging;
  • Register access, DMA access, and IRQ handling.




ASYGN — AIDGE CHALLENGE — SMART VIDEO SENSOR                                                                             4
<PARSED TEXT FOR PAGE: 5 / 12>
 4. Objectives and scoring criteria
 Your objective for this challenge is to design a people-counting system using the ColibryNPU chip with an energy
 budget below 5 mW for the entire system.

 The image below presents an example of people counting.




 At the end of the challenge, you will have to submit:
  • A technical report in the form of a PDF presentation, in French or English, detailing the methodological approach, the
    solution developed, contributions to AIDGE, and the results obtained;
  • A demonstration video recorded as proof of correct operation, highlighting the solution and the results.
 As jury members, we will pay particular attention to the performance, framerate, and power consumption of your
 system. We encourage you to achieve consistency between these three criteria rather than prioritizing one at the
 expense of the other two.

 The table below shows the level assigned according to your system's performance.

                  Level                                             Performance

                  Legendary                                         > 0.98

                  Gold                                              > 0.95

                  Silver                                            > 0.90

                  Bronze                                            > 0.80



 The table below shows the level assigned according to your system's framerate.

                  Level                                             Framerate

                  Legendary                                         > 15 FPS

                  Gold                                              > 10 FPS

                  Silver                                            > 5 FPS

                  Bronze                                            > 1 FPS




ASYGN — AIDGE CHALLENGE — SMART VIDEO SENSOR                                                                                 5
<PARSED TEXT FOR PAGE: 6 / 12>
 The table below shows the level assigned according to your system's power consumption.

                   Level                                              Power consumption

                   Legendary                                          < 2 mW

                   Gold                                               < 5 mW

                   Silver                                             < 10 mW

                   Bronze                                             < 15 mW



 We will also award a score for the methodology you followed. To this end, we ask you to emphasize the content of
 your dataset, the graph of your neural network, and the sequencing of your complete processing chain.

 Finally, we will award a final score for the clarity and quality of the presentation video and technical report.

 5. Schedule
 The challenge will take place in 3 phases followed by an evaluation, as shown in the image below.




 Below is the detailed content of Phase 1.

 Phase 1 — Getting started with the AIDGE platform and designing the people-detection
 model
 Duration: From 01/10/2026 to 05/11/2026

 Access to resources: Phase 1 mission book.




ASYGN — AIDGE CHALLENGE — SMART VIDEO SENSOR                                                                        6
<PARSED TEXT FOR PAGE: 7 / 12>
 AIDGE platform
 (Installation guide, wiki, tutorials, Chat, Issues)

 ASYGN examples
 (HelloWorld, BikeCounter)

 Dataset
 (COCO Person subset)

 Objectives
  • Install the AIDGE platform and go through the tutorials.
  • Run and analyze the ASYGN examples.
  • Design your dataset (Training and Validation).
  • Train an FP32 model with the AIDGE platform compatible with the operators supported by ColibryNPU.
  • Optimize and quantize the FP32 model to UINT32 with the AIDGE platform in order to make it compatible with
    ColibryNPU's memory capacity.

 Phase 2 — Getting started with the ColibryNPU evaluation kit and initial field tests
 Duration: From 05/11/2026 to 03/12/2026

 Access to resources: Phase 2 roadmap.

  • Colibry Tiny Evalboard and Colibry Probe boards.
  • Colibry Tiny Evalboard user manual.
  • ColibryNPU chip datasheet.
  • Use the Colibry Tiny Evalboard and Colibry Probe boards to acquire an image stream and display it on the computer.
  • Use the Colibry Tiny Evalboard to transmit information to a PC using the Bluetooth BLE module.
  • Export the UINT8 model for ColibryNPU with the AIDGE platform.
  • Design and deploy an image-processing chain in ColibryNPU and integrate the model call into this chain. (Image
    acquisition → AI inference → post-processing → BLE transfer of results.)

 Phase 3 — Detector performance optimization
 Duration: From 03/12/2026 to 31/12/2026

 Access to resources: Phase 3 roadmap.

 Objectives: Improve and optimize the model and processing chain to achieve the best performance, framerate, and
 power consumption.




ASYGN — AIDGE CHALLENGE — SMART VIDEO SENSOR                                                                             7
<PARSED TEXT FOR PAGE: 8 / 12>
 6. Support
 Throughout the challenge, you should be able to organize your work with your teammates and search for information
 to solve the difficulties you encounter.

 However, you may contact your tutor and the AIDGE and ASYGN teams when:
  • the difficulty encountered cannot be solved independently;
  • the information you are looking for is not available in the shared resources or in the scientific literature.
 Important note: we encourage you to contribute to the AIDGE platform if you identify missing features or
 improvements!

 AIDGE
 The AIDGE team will be available through the project's GitLab issues or via this Chat:
 https://chat.eclipse.org/#/room/#eclipse-projects:matrix.eclipse.org

 ASYGN
 The ASYGN team will be available through the issues of the aidge_export_colibry GitLab project (you will need to
 create an Eclipse account; you can tick the confidential box so that your issue is private):
 https://gitlab.eclipse.org/eclipse/aidge/aidge_export_colibry/-/work_items/new?description_template=Challenge_Aidge

 Creating a private issue for the ASYGN team

 Follow-up meetings
 The follow-up meetings shown below will allow you to ask questions, raise important blockers, and share your
 experience with the other students. They will be led by Adrien from ASYGN. Their duration will be 1.5 hours.




ASYGN — AIDGE CHALLENGE — SMART VIDEO SENSOR                                                                           8
<PARSED TEXT FOR PAGE: 9 / 12>
Date            Time      Content                                   Link

Thu. 8/10/26    14:30     Topic presentation — Smart Video Sensor

Thu. 5/11/26    14:30     Follow-up meeting and end of Phase 1      https://teams.microsoft.com/meet/3309403460894
                                                                    459?p=KIb8DYT6LsXbt6xCsa

Thu. 19/11/26   14:30     Follow-up meeting                         https://teams.microsoft.com/meet/3539343615136
                                                                    478?p=QTUmKxdpaUBMcAYks0

Thu. 3/12/26    14:30     Follow-up meeting and end of Phase 2      https://teams.microsoft.com/meet/3539343615136
                                                                    478?p=QTUmKxdpaUBMcAYks0

Thu. 17/12/26   14:30     Follow-up meeting                         https://teams.microsoft.com/meet/3523878429671
                                                                    270?p=ydcmHNhVJgbXqnXIP6




ASYGN — AIDGE CHALLENGE — SMART VIDEO SENSOR                                                                         9
<PARSED TEXT FOR PAGE: 10 / 12>
 7. AIDGE design flow
 Throughout this challenge you will discover the different stages of the design flow for embedded software integrating
 artificial intelligence. These stages are presented below:




 Your first step will be to create your dataset. You may use a public dataset or create your own dataset (more
 information in Section 8).

 You will then use the AIDGE platform to train your neural network, optimize it, quantize it, and then export it for
 ColibryNPU. Check that you do not lose too much performance after each step. The image below illustrates a
 comparison of the performance of a bicycle-detection model at each stage.




 Also check that your model is compatible with the hardware constraints of ColibryNPU (more information in Section 8).

 Once your validated quantized model has been exported, you will have to design the application firmware that will run
 on the ColibryNPU microcontroller. This MCU is used to orchestrate the hardware peripherals and perform
 post-processing (filtering, tracking, etc.). Your neural network will be executed by the dedicated hardware accelerator
 (NNPA), which will provide the results of each inference to the MCU via IRQ.

 Finally, you will compile your firmware and program ColibryNPU to test your system in the field.

 We present all these steps in a very linear way. However, you will naturally need to go back to previous steps to
 correct, adjust, and optimize your system. After several weeks of development, your perspective on the project will be
 different. You will probably want to revisit your dataset to improve your overall performance!

 We would like to draw your attention to the optimization step of your neural network. This step must make it fully
 compatible with ColibryNPU. A compatibility check will be performed after quantization to verify the layers used by your
 network and its memory footprint (weights + data < 640 kB).




ASYGN — AIDGE CHALLENGE — SMART VIDEO SENSOR                                                                               10
<PARSED TEXT FOR PAGE: 11 / 12>
 8. Advice for Phase 1
 Play with and understand the ASYGN examples in AIDGE
 To get started, we invite you to familiarize yourself with the ASYGN examples available in the aidge_export_colibry
 project:
  • HelloWorld (sine prediction):
    https://gitlab.eclipse.org/eclipse/aidge/aidge_export_colibry/-/tree/main/examples/10_hello_world?ref_type=heads
  • BikeCounter:
    https://gitlab.eclipse.org/eclipse/aidge/aidge_export_colibry/-/tree/main/examples/30_bike_vision?ref_type=heads
 The two examples listed above indicate, in their logs, the model characteristics and their compatibility with ColibryNPU.
 We draw your attention to the following logs:

 INFO: The NNPA model configuration uses 132128 bytes, 8258 words, 9 buffers.
 INFO: Inference requires 9641484 cycles
 INFO: Power Consumption (rough estimation, VDD=0.8V): 521.6 uJ per inference
 INFO: The NNPA model configuration + data fits the Colibry memory constraints

 Note: The 640 kB of NNPA memory available on ColibryNPU is divided into 40 buffers of 16 kB.

 Choose the system's use scenario
 Before starting to design your neural network, we recommend choosing the use scenario for your people counter. This
 scenario should be simple and will provide you with an initial framework throughout your development. We recommend
 detecting a stream of people walking in single file, spaced 50 cm apart and walking in the same direction. We advise
 against trying to detect groups of people. It is important to first establish a functional reference using a simple scenario.
 You can then add complexity and variability to your scenario at the end of the challenge if you have time remaining.

 Use the COCO Person subset
 We recommend using the COCO Person subset to train your first model. This will allow you to validate your
 neural-network training flow. You can then adapt this dataset or create your own dataset so that your model learns
 from images close to your use scenario. This is an optimization lever because a model trained on data close to the
 final scenario will be optimized in terms of size.

 You can also use tools to perform data augmentation or generate synthetic images. For example, you can capture
 video frames from a video game. By degrading the images and converting them to grayscale, you should be able to
 get closer to the image you will obtain from the camera.




ASYGN — AIDGE CHALLENGE — SMART VIDEO SENSOR                                                                                 11
<PARSED TEXT FOR PAGE: 12 / 12>
 Define the image characteristics of the dataset
 You can now choose the resolution of your people counter. This resolution must not exceed 320×240 px (QVGA). You
 can absolutely choose a smaller resolution such as QQVGA (160×120 px), as this is a lever for reducing the size of
 your model.

 We recommend training your neural network on grayscale images.

 Choose a model architecture compatible with ColibryNPU
 Your neural-network architecture must be compatible with ColibryNPU. The list of supported operators is provided
 below:

  • Pointwise & Depthwise Convolution (Conv2D);
  • Dense;
  • Average/Max & GlobalAverage Pooling, Batch Normalization;
  • Add;
  • Linear/ReLU Activation.
 Operators not included in the list above cannot naturally be executed by ColibryNPU. It will be possible to split your
 network sequence by executing unsupported layers on the microcontroller, but we advise against using this approach
 in the challenge.

 Quantize the model with AIDGE
 You will need to perform asymmetric per-tensor uint8 quantization to quantize your neural network for ColibryNPU.

 Take into account the memory available in ColibryNPU
 Once you have quantized your neural network to UINT8, the model (network weights) + data (image) must be less
 than 640 kB because this is the size of the memory reserved in ColibryNPU for executing your model.




ASYGN — AIDGE CHALLENGE — SMART VIDEO SENSOR                                                                              12
