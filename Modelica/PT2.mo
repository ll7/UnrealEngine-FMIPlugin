model PT2
  Modelica.Blocks.Continuous.SecondOrder secondOrder(D = 0.5, initType = Modelica.Blocks.Types.Init.NoInit, k = 1, w = 1)  annotation(
    Placement(visible = true, transformation(origin = {0, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  output Modelica.Blocks.Interfaces.RealOutput y annotation(
    Placement(visible = true, transformation(origin = {26, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 180), iconTransformation(origin = {56, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Sources.Pulse pulse(period = 1, width = 25)  annotation(
    Placement(visible = true, transformation(origin = {-54, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
equation
  connect(secondOrder.y, y) annotation(
    Line(points = {{12, 0}, {26, 0}}, color = {0, 0, 127}));
  connect(secondOrder.u, pulse.y) annotation(
    Line(points = {{-12, 0}, {-42, 0}}, color = {0, 0, 127}));
  annotation(
    uses(Modelica(version = "4.0.0")));
end PT2;