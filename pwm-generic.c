#include <linux/module.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/pwm.h>
#include <linux/slab.h>
#include <linux/bitops.h>
#include <linux/platform_device.h>

struct generic_pwm
{
	struct pwm_chip chip;
	void __iomem *io_base;
};

static inline struct generic_pwm *get_generic_pwm(struct pwm_chip *chip)
{
	return container_of(chip, struct generic_pwm, chip);
}

static int generic_pwm_request(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct generic_pwm *data = get_generic_pwm(chip);

	dev_dbg(pwm->chip->dev, "into %s\n", __func__);

	return 0;
}

static void generic_pwm_free(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct generic_pwm *data = get_generic_pwm(chip);

	dev_dbg(pwm->chip->dev, "into %s\n", __func__);
}

static int generic_pwm_config(struct pwm_chip *chip, struct pwm_device *pwm,
							  int duty_ns, int period_ns)
{
	struct generic_pwm *data = get_generic_pwm(chip);

	dev_dbg(pwm->chip->dev, "into %s\n", __func__);

	return 0;
}

static int generic_set_polarity(struct pwm_chip *chip, struct pwm_device *pwm,
								enum pwm_polarity polarity)
{
	struct generic_pwm *data = get_generic_pwm(chip);

	dev_dbg(pwm->chip->dev, "into %s\n", __func__);

	return 0;
}

static int generic_pwm_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct generic_pwm *data = get_generic_pwm(chip);

	dev_dbg(pwm->chip->dev, "into %s\n", __func__);

	return 0;
}

static void generic_pwm_disable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct generic_pwm *data = get_generic_pwm(chip);

	dev_dbg(pwm->chip->dev, "into %s\n", __func__);
}

static const struct pwm_ops generic_pwm_ops = {
	.request = generic_pwm_request,
	.free = generic_pwm_free,
	.config = generic_pwm_config,
	.enable = generic_pwm_enable,
	.disable = generic_pwm_disable,
	.set_polarity = generic_set_polarity,
	.owner = THIS_MODULE,
};

static int generic_pwm_probe(struct platform_device *pdev)
{
	struct generic_pwm *data;
	struct device_node *np = pdev->dev.of_node;
	int err;

	data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
	if (data == NULL)
		return -ENOMEM;

	data->io_base =
		devm_ioremap(&pdev->dev, pdev->resource->start,
					 pdev->resource->end - pdev->resource->start + 1);
	if (data->io_base == NULL)
		return -EIO;

	if (of_find_property(np, "pwm-channel", NULL))
		of_property_read_u32(np, "pwm-channel", &data->chip.npwm);
	else
		data->chip.npwm = 1;

	data->chip.dev = &pdev->dev;
	data->chip.ops = &generic_pwm_ops;
	data->chip.base = pdev->id;

	err = pwmchip_add(&data->chip);
	if (err < 0)
		return err;

	dev_info(&pdev->dev, "generic-pwm probe successful\n");
	platform_set_drvdata(pdev, data);

	return 0;
}

static int generic_pwm_remove(struct platform_device *pdev)
{
	struct generic_pwm *data = platform_get_drvdata(pdev);
	int err;

	err = pwmchip_remove(&data->chip);
	if (err < 0)
		return err;

	dev_dbg(&pdev->dev, "pwm driver removed\n");

	return 0;
}

static const struct of_device_id generic_pwm_ids[] = {
	{.compatible = "generic-pwm"},
	{/* sentinel */},
};

static struct platform_driver generic_pwm_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "generic-pwm",
		.of_match_table = generic_pwm_ids,
	},
	.probe = generic_pwm_probe,
	.remove = generic_pwm_remove,
};
module_platform_driver(generic_pwm_driver);

MODULE_AUTHOR("Chowe");
MODULE_DESCRIPTION("generic PWM driver for common");
MODULE_LICENSE("GPL v2");
