model Radaufhaengung
  Modelica.Mechanics.Translational.Components.SpringDamper springDamper(s_rel(start = 5), s_rel0 = 0.5)  annotation(
    Placement(visible = true, transformation(origin = {-66, 10}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Mechanics.Translational.Components.Fixed fixed annotation(
    Placement(visible = true, transformation(origin = {-82, 0}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Mechanics.Translational.Components.Mass mass(m = 1)  annotation(
    Placement(visible = true, transformation(origin = {-40, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
equation
  connect(fixed.flange, springDamper.flange_a) annotation(
    Line(points = {{-82, 0}, {-79, 0}, {-79, 10}, {-76, 10}}, color = {0, 127, 0}));
  connect(mass.flange_a, springDamper.flange_b) annotation(
    Line(points = {{-50, 0}, {-53, 0}, {-53, 10}, {-56, 10}}, color = {0, 127, 0}));
  annotation(
    uses(Modelica(version = "3.2.3")));
end Radaufhaengung;