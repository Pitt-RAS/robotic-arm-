From Computational Modeling and Simulation for Engineers Class

1. Follow the instructions at the links below to install Python, Jupyter, and VPython:
a. The Anaconda Python distribution is recommended – ensure that you have Python
3.7 or later: https://www.anaconda.com/products/individual
b. It is highly recommended that you use Python virtual environments, either through
Anaconda navigator or conda (installed with Anaconda). Create an environment that
uses Python 3.7, and confirm the version by activating the environment and running
‘python --version':
i. Managing environments with Conda:
https://docs.conda.io/projects/conda/en/latest/user-guide/tasks/manageenvironments.
html
ii. Managing environments with Anaconda navigator:
https://docs.anaconda.com/anaconda/navigator/tutorials/manageenvironments/
c. Install Jupyter notebook – the “classic” notebook is generally simpler to set up with
VPython, but you are welcome to try JupyterLab as well. Follow instructions under
“Getting started with the classic Jupyter Notebook” at the following link, and install
using conda:
i. https://jupyter.org/install
ii. See here for more information on Jupyter notebooks: https://jupyternotebook.
readthedocs.io/en/stable/notebook.html
d. Install and set up VPython following the instructions at the following link at the top
of the page (install VPython using conda):
https://vpython.org/presentation2018/install.html


my version 

https://www.anaconda.com/products/distribution

https://docs.anaconda.com/anaconda/install/windows/

leave both advanced options as uncheked. 

after the install search "anaconda prompt" 
notice the word (base) before your path to CWD. this is a conda environemnt.
we can deactivate it by running 
conda deactivate

now (base) has been removed. 

make a new virtual environment specifying the version of python you have. 
i have python 3.9.2

so i will run conda create -n ENV_NAME python=3.9.2
note i think this has to be greater than python 3.7

to activate the env, run
conda activate ENV_NAME

now you should see (ENV_NAME) at front of path.

next you need to install vpython (required) and jupyter notebook (recommended)

run 
conda install -c conda-forge vpython

and 
conda install -c conda-forge notebook 
notebook may already be installed. 

you can install other handy jupyter notebook things if you want. thats detailed here https://towardsdatascience.com/how-to-set-up-anaconda-and-jupyter-notebook-the-right-way-de3b7623ea4a

and now you want to navigate to the directory where you will workin on your code. 
once you are there run
jupyter notebook

put the following code in a cell
from vpython import*

scene = canvas()

box = box(color=color.red)

and run it and see if it works!

