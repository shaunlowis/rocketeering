"""
Flight path visualisation
- Simple example with a set arc shape.
- More complex data can be visualised as a point cloud:
https://deckgl.readthedocs.io/en/latest/gallery/point_cloud_layer.html#pointcloudlayer
"""

import pydeck as pdk
import pandas as pd

# Coringa golf club
start_coords = (-43.4633061237156, 172.49099147719105)
# Just over the waimak river
end_coords = (-43.41614487464603, 172.47836707684)


df = pd.DataFrame(
    {
    "start_coords_lat": [start_coords[0]], 
    "start_coords_lon": [start_coords[1]], 
    "end_coords_lat": [end_coords[0]],
    "end_coords_lon": [end_coords[1]],
    }
    )

GREEN_RGB = [0, 255, 0, 40]
RED_RGB = [240, 100, 0, 40]

# Specify a deck.gl ArcLayer:
# Docs: https://deck.gl/docs/api-reference/layers/arc-layer

arc_layer = pdk.Layer(
    "ArcLayer",
    data=df,
    get_width=5,
    get_source_position=["start_coords_lon", "start_coords_lat"],
    get_target_position=["end_coords_lon", "end_coords_lat"],
    get_tilt=15,
    get_source_color=RED_RGB,
    get_target_color=GREEN_RGB,
    pickable=True,
    auto_highlight=True,
)

view_state = pdk.ViewState(
    latitude=-43.4633061237156,
    longitude=172.49099147719105,
    bearing=45,
    pitch=50,
    zoom=8,
)


TOOLTIP_TEXT = {"html": "{S000} Example <br /> flight path over the Waimak."}
r = pdk.Deck(arc_layer, initial_view_state=view_state, tooltip=TOOLTIP_TEXT)
r.to_html("post-processing/waimak_flight.html")