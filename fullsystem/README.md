Achievement
Tell us about the work of which you are most proud and how those accomplishements are meaningful to you
How solid do you feel on your achievement of the course learning goals?

I am most proud of my work in clock assignment because it was good to expand out of my comfort zone to learn about how to efficiently use circuitry to connect a ticking time.
It was the core hardware software integration that let me understand the relationship between the gpio pins and the wiring on the breadboard with the codebase. 
In addition, it was very rewarding to set up a .h file as an extension because it allowed for very organized and transportable code. I am going to use the header files in my final project to make and isolate the components of sensor detection and graphics library. So learning how to do this in assign2 extension is not only helpful for the final project, but also for the future workflow and pipeline. It is good to have a file that has a collection of implemented functions to include so that the code is cleaner
and the process of integration of sensors and the graphic interface is so much smoother. 


Effort, engagement
Tell us about what goes on behind the scenes:
Persistence through tough problem-solving
Working through hard debugging challenges
Multiple rewrites to get something right
Side quests to explore additional material or further your mastery
Teaching and learning with your peers, community-building

When coding my algorithms, it was important to first read and understand exactly the objective to break it into parts. There were many instances where I thought the end product wasn't actually the same as the spec. For example, I remember starting and having an early draft of printf very early, when I realized strlcat was supposed to return the expected length, no matter the bufsize. Then I had to change many iterations of code to account for this. In addition, for malloc, I wasn't sure whether we should point to the area of free space, or to the header. To make it work, I added 8bytes to every header of the heapworkflow so that it tested. Then during this week of retests, Julie helped me realize that heap workflow should not be changed and the code itself should refer to the empty space. This helped me in two ways, 1) when I started the homework assignment to be extra careful of what it is asking by jotting down the input/output and the workflow, and 2) when run into a bug, to slow down and refer back to the assignment guidelines to acknowledge the confusion to see where I may have deviated from the original goal.  In hard debugging challenges, it was important to take breaks but WITHOUT a phone or electronic. I think it refreshed my thinking process without being distracted by social media traps. In addition, talking out my ideas to articulate my brain pattern was very helpful. It is an investment to fully understand the instructions without trying to rush into the feeling of writing a piece of code first. I remember late nights in Gates Basement, microwaving a bag of popcorn and debugging with my new friends. I feel like I've learned so much about my coding style through helping debug some framebuffers so that I understand my own framebuffer, as well as the help of other friends in the lab. I've met so many passionate and hardworking people here which inspires me to build things I love. I am genuinely grateful for the opportunity to take this class for all of the people I've met and all the projects I've built. 



Growth
When did something finally "click" for you?
How are you a more capable engineer now than at the start?
Share a moment where you surprised yourself or didn't give up when it was hard


One obvious time it clicked was when I put the final system together and ran the console with NO guardrails. It felt like taking the training wheels off of a bike after assembling all of the parts together. But before that, was when I implemented teh terminal in the keyboard module, with the history function. It felt surreal to make a terminal of my own and be able to access address points. Once I even confused the terminal I ran it in with the one I made, what a good problem to have!! I feel like I've grown as an engineer because of all of the persistence of debugging the code  but more than that, creating my own guide to bitwise operations, understanding state machines, and converting assembly to C. One time that I surprised myself was when I messed up the functionality of my key_event_t and proceeded to my scancode, only to realize that I had accidentally recorded the press and release twice each time. At this point, it was simply easier to restart the function than to adjust it. So, after taking a small break I sat down and I created a comprehensive version of it. Instead of feeling hopeless that my event function didn't work after hours of iterating, I shook off my disappointment and created a better version and versatile part of the code. 
