FROM python:3.11
ADD pygl pygl/
ADD models models/
# RUN apt install 
WORKDIR pygl
RUN python -m pip install -r requirements.txt
ENTRYPOINT [ "./teapot_gpu_bezier.py" ] 