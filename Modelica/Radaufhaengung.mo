model Radaufhaengung
  Modelica.Mechanics.Translational.Components.SpringDamper springDamper(s_rel(start = 5), s_rel0 = 0.5)  annotation(
    Placement(visible = true, transformation(origin = {-50, 50}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Mechanics.Translational.Components.Fixed fixed annotation(
    Placement(visible = true, transformation(origin = {-50, 70}, extent = {{-10, -10}, {10, 10}}, rotation = 180)));
  Modelica.Mechanics.Translational.Components.Mass mass(m = 0.1)  annotation(
    Placement(visible = true, transformation(origin = {-50, 10}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
equation
  connect(fixed.flange, springDamper.flange_a) annotation(
    Line(points = {{-50, 70}, {-50, 60}}, color = {0, 127, 0}));
  connect(springDamper.flange_b, mass.flange_a) annotation(
    Line(points = {{-50, 40}, {-50, 20}}, color = {0, 127, 0}));
  annotation(
    uses(Modelica(version = "3.2.3")));
end Radaufhaengung;