model RadaufhaengungNeu
  Modelica.Mechanics.MultiBody.Parts.PointMass pointMass(m = 0.1)  annotation(
    Placement(visible = true, transformation(origin = {42, 40}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Mechanics.MultiBody.Forces.SpringDamperParallel springDamperParallel(c = 10, d = 2, s_unstretched = 0.5)  annotation(
    Placement(visible = true, transformation(origin = {16, 40}, extent = {{10, -10}, {-10, 10}}, rotation = 0)));
  Modelica.Mechanics.MultiBody.Parts.Fixed fixed1 annotation(
    Placement(visible = true, transformation(origin = {-10, 40}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Mechanics.MultiBody.Joints.Prismatic prismatic(s(fixed = true, start = 10))  annotation(
    Placement(visible = true, transformation(origin = {16, 66}, extent = {{10, -10}, {-10, 10}}, rotation = 0)));
equation
  connect(pointMass.frame_a, springDamperParallel.frame_a) annotation(
    Line(points = {{42, 40}, {26, 40}}, color = {95, 95, 95}));
  connect(fixed1.frame_b, springDamperParallel.frame_b) annotation(
    Line(points = {{0, 40}, {6, 40}}, color = {95, 95, 95}));
  connect(springDamperParallel.frame_a, prismatic.frame_a) annotation(
    Line(points = {{26, 40}, {26, 66}}, color = {95, 95, 95}));
  connect(prismatic.frame_b, springDamperParallel.frame_b) annotation(
    Line(points = {{6, 66}, {6, 40}}, color = {95, 95, 95}));
  annotation(
    uses(Modelica(version = "3.2.3")));
end RadaufhaengungNeu;