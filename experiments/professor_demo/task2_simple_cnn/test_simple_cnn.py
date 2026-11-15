"""Small architecture checks for the professor CNN."""

import unittest

from experiments.professor_demo.task2_simple_cnn.simple_cnn import build_model


class ProfessorSimpleCNNTests(unittest.TestCase):
    def test_model_has_exactly_two_trainable_layers(self):
        model = build_model()
        trainable_layers = [
            layer.name for layer in model.layers if layer.trainable_weights
        ]
        self.assertEqual(trainable_layers, ["conv2d", "person_probability"])

    def test_input_and_output_shapes(self):
        model = build_model()
        self.assertEqual(tuple(model.input_shape[1:]), (32, 32, 1))
        self.assertEqual(tuple(model.output_shape[1:]), (1,))


if __name__ == "__main__":
    unittest.main()
