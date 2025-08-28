# Master's thesis plan for Felix Kalchschmid

## Topic: Semantic Abstraction of Scenes

## Method
The thesis will be implementing an interactive system in Unity that supports the user in the creation of abstract paintings. The system is implemented as a modeling process that is based off of high quality assets. The abstraction process is both visual based on the image data and geometric based on the scene data. 

The main inspirations are the Sketching in Gestalt space paper that allow for geometric scene abstraction based on the Gestalt principles similarity, regularity and proximity. 

Also the paper Semantic Image Abstraction using Panoptic Segmentation for Robotic Painting is the main inspiration on the impact of Semantics on the abstraction process. 


## Data
We have the project from the Sketching in gestalt space paper available and are trying to get it running in the next weeks.

The paper included some user study results. These can serve as the basis of how well this system is perceived to work. They can also be reevaluated and new questions can be asked.

The new system can also be compared against the old system


## Status of knowledge
There is not much literature on abstraction of 3D models. A bit more maybe on 2D images, or geometry but 3d is really what i am interested in. 

There are solid systems to do semantic rendering like Omniverse Replicator or BlenderProc these just have to be called with a scene description and a few camera points and they give you all the multi-dimensional data you need.

Other than the few main papers there is not much, this is also an opportunity to cover some basic grounds

There is a bit more on semantic modeling that might be interesting

i also dont have a full overview yet
## Outline
### Introduction 
A discussion on why semantics are necessary for abstraction. Also how semantics are used in painting and art and how you can perceive how artists abstract different types of objects differently.

### Model for Semantic Abstraction

Here i will theorise the concept of adding Semantics to the abstraction process of scenes as part of a pipeline. Here i have to use the literature and define this process and argue why and how this could work.

### Implementation
Design and implement a editor extension for Unity and a Runtime Kiosk demo that implements each stage of the interactive abstraction pipeline. Create an installable package that includes both Documentation, Demo scenes and a guided way to use the program. 

Implement an adapter to the robotic painting system to allow the creation of real abstract paintings. 

The abstract paintings created with these methods can be compared to abstract paintings by historic artists like Gabriele Münther who also painted actual scenes but in an abstracted way.
### Discussion

Show it's usefulness with some images created using the system.

Compare the system to the Gestalt sketching system. Create a user study evaluating the modeling process on some metrics.

## References

## To be filled in by the supervisor
I think this is acceptable / not acceptable.

Other comments:

______________
Signed by Prof. Dr. Oliver Deussen



# Improvements

Alright, Felix — I think you’re in a pretty good spot already, but your current draft can become much stronger and clearer if we make a few structural and content improvements.

Right now, your plan communicates your *intention* well, but it leaves some open questions about **what exactly** you want to achieve, **how you’ll evaluate success**, and **what your unique contribution** will be compared to the prior work (*Sketching in Gestalt Space*, semantic abstraction papers, BlenderProc, etc.).

I'll give you feedback in three layers:

---

## **1. Strategic feedback**

### Where your plan is strong

* You’ve clearly connected your topic to existing research (Gestalt + semantics + abstraction).
* You’ve already identified two main inspiration papers.
* You’re aware of available data (Gestalt code + semantic pipelines).
* You have a general outline for implementation and evaluation.

### Where you need more clarity

* **Research question(s)**: The plan doesn’t state a contestable claim yet.

  * Do you want to *prove* that semantic abstraction produces better results than Gestalt-only abstraction?
  * Do you want to *develop a new abstraction pipeline* and evaluate user experience?
  * Do you want to *study the role of semantics in abstraction* theoretically and practically?
* **Evaluation strategy**: You mention “comparing to the old system” and “user studies,” but this part should be more concrete.

  * What are the evaluation criteria? (e.g., usability, aesthetic quality, accuracy of abstraction, similarity to artistic styles)
* **Scope & feasibility**: The plan currently includes **a lot** (Unity system + robotic painting integration + user study + art comparison). You might want to frame robotic painting as a *bonus* or “optional demonstration,” unless it’s essential.
* **Status of knowledge**: Needs more detail and references — ideally 3–5 key works, with a short summary of what they do and how yours differs.

---

## **2. Suggested content refinements**

Here’s how we could improve each section:

---

### **Topic**

> Semantic Abstraction of 3D Scenes for Interactive Artistic Rendering

Add 2–3 sentences that define:

* The **problem**: current systems lack semantic awareness, leading to less artistically meaningful abstractions.
* The **goal**: develop a system that combines Gestalt-based geometric abstraction with semantic information to better reflect how artists selectively simplify scenes.
* The **research question**:
  *“How can semantic scene information improve the quality and controllability of geometric abstraction for artistic purposes?”*

---

### **Methods**

Currently good, but let’s make it sharper:

* **System design**: Implement an interactive Unity-based modeling tool.
* **Pipeline**:

  1. **Scene ingestion**: Use high-quality assets.
  2. **Semantic labeling**: Integrate tools like BlenderProc / Omniverse Replicator.
  3. **Gestalt-based geometry simplification**: Build upon *Sketching in Gestalt Space*.
  4. **Semantic-driven abstraction strategies**: Different abstraction rules depending on object class.
  5. **Output**: Generate stylized 2D abstractions and, optionally, robotic-painted outputs.
* **Evaluation**:

  * Compare the new pipeline with the Gestalt-only pipeline.
  * Conduct a small-scale user study (5–10 participants) to measure **usability** and **aesthetic preference**.

---

### **Data**

* **Primary source**: Existing Gestalt system + included study results.
* **Semantic segmentation**: BlenderProc or Omniverse Replicator will generate multi-view semantic masks.
* **Evaluation data**:

  * Scenes from the Gestalt dataset.
  * At least one real-world environment.
* Optional: datasets like **SceneNet RGB-D** or **Replica** if needed.

---

### **Status of knowledge**

This needs a bit more structure. Suggested framing:

* **Gestalt-based abstraction**

  * *“Sketching in Gestalt Space”* (Strothotte et al.) — core method for geometric simplification.
* **Semantic image abstraction**

  * *“Semantic Image Abstraction using Panoptic Segmentation for Robotic Painting”* — adds semantic awareness but focuses on 2D.
* **Semantic rendering pipelines**

  * Omniverse Replicator / BlenderProc — data sources, not abstraction methods.
* **Gap**

  * No existing system combines **semantic awareness** + **3D geometric abstraction** + **interactive artistic control**.

---

### **Outline**

You already have the structure, but I’d make it more outcome-driven:

1. **Introduction**

   * Motivation: Why abstraction + semantics matter.
   * Artistic context: How painters abstract scenes differently.
2. **Related Work**

   * Gestalt-based abstraction.
   * Semantic-aware image simplification.
   * Robotic painting & data-driven art.
3. **Model for Semantic Abstraction**

   * Conceptual pipeline.
   * How semantics influence abstraction rules.
4. **Implementation**

   * Unity editor + runtime.
   * Semantic integration.
   * Output formats.
5. **Evaluation**

   * User study design.
   * Comparison against Gestalt-only baseline.
   * Qualitative + quantitative results.
6. **Discussion**

   * Usefulness and limitations.
   * Future work: robotic painting, integration into Omniverse, etc.
7. **Conclusion**

---

### **References**

Aim for 5–7 strong initial references:

* Strothotte et al. — Sketching in Gestalt Space.
* Semantic Image Abstraction (robotic painting).
* Omniverse Replicator docs.
* BlenderProc paper.
* Any reference on artistic abstraction theory.

---

## **3. Next steps**

If you want, I can:

* Draft a **clean, polished version** of your plan with better phrasing.
* Suggest a **set of references** for the “status of knowledge” section.
* Create a **visual diagram** of your planned pipeline.
* Help you define **clear research questions** and **evaluation metrics**.

---

Do you want me to first focus on rewriting your current plan into a more polished and structured draft, or should we first clarify your **exact research question**?
I’d recommend locking that down first — it’ll guide everything else.

