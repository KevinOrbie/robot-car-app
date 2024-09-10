import argparse
import plotly.graph_objects as go
from plotly.subplots import make_subplots

# ----------------------------------- Parse Arguments -----------------------------------
# Define the parser
parser = argparse.ArgumentParser(description='Visualize top performance data.')

# Declare an argument
parser.add_argument('filename', help='top log file to process.')

# Parse the command line arguments
args = parser.parse_args()


# ----------------------------------- Parse Log File ------------------------------------
data = {}
headers = []

with open(args.filename, 'r') as file: # open the file
    for line in file:
        # Ignore empty lines
        if line.rstrip() == "":
            continue

        # Parse Line
        values = line.rsplit()

        if not values[0].isdigit():
            # Find header values
            headers = values
            continue

        # Add value data
        for idx in range(len(headers)):
            value = None

            # Convert values to float if possible
            try:
                value = float(values[idx])
            except ValueError:
                value = values[idx]

            if headers[idx] in data:
                data[headers[idx]].append(value)
            else:
                data[headers[idx]] = [value]


# ------------------------------------- Plot Data ---------------------------------------
# Initialize figure with subplots
fig = make_subplots(
    rows=2, cols=1,
    specs=[[{"type": "scatter"}], [{"type": "scatter"}]]
)

# Draw percentual plot
perc = ['%CPU', '%MEM']
for key in perc:
    if key in data:
        fig.add_trace(
            go.Scatter(
                x=list(range(len(data[key]))), 
                y=[x for x in data[key]], # From KB to Bytes
                mode='lines+markers',
                name=key,
                legendgroup=2
            ), row=1, col=1
        )
fig.update_xaxes(title_text="sample", row=1, col=1)
fig.update_yaxes(title_text="Usage [%]", row=1, col=1)

# Draw Memory Plot
memory = ['RES', 'SWAP', 'VIRT', 'SHR', 'DATA', 'CODE', 'USED']
for key in memory:
    if key in data:
        fig.add_trace(
            go.Scatter(
                x=list(range(len(data[key]))), 
                y=[x * 1000 for x in data[key]], # From KB to Bytes
                mode='lines+markers',
                name=key,
                legendgroup=1
            ), row=2, col=1
        )
fig.update_xaxes(title_text="sample", row=2, col=1)
fig.update_yaxes(title_text="Memory [Bytes]", row=2, col=1)

# Update Style
fig.update_layout(
    template="plotly_dark",
    annotations=[
        dict(
            text="",
            showarrow=False,
            xref="paper",
            yref="paper",
            x=0,
            y=0)
    ],
    legend_tracegroupgap=180,
    legend_groupclick="toggleitem"
)

fig.show()

