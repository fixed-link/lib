#include <linux/module.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/pwm.h>
#include <linux/slab.h>
#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/platform_device.h>

struct generic_clk {
	struct clk_hw hw;
	void __iomem *clk_io;
};

static inline struct generic_clk *to_generic_clk(struct clk_hw *hw)
{
	return container_of(hw, struct generic_clk, hw);
}

static int generic_clk_enable(struct clk_hw *hw)
{
	struct generic_clk *clk = to_generic_clk(hw);

	// todo

	return 0;
}

static void generic_clk_disable(struct clk_hw *hw)
{
	struct generic_clk *clk = to_generic_clk(hw);

	// todo
}

static int generic_clk_is_enabled(struct clk_hw *hw)
{
	struct generic_clk *clk = to_generic_clk(hw);

	// todo

	return 0;
}

static const struct clk_ops generic_clk_ops = {
	.enable = generic_clk_enable,
	.disable = generic_clk_disable,
	.is_enabled = generic_clk_is_enabled,
};

static int generic_clk_probe(struct platform_device *pdev)
{
	struct clk *clk;
	struct generic_clk *data;
	struct clk_init_data init = {};
	struct device_node *np = pdev->dev.of_node;

	data = devm_kzalloc(&pdev->dev, sizeof(struct generic_clk), GFP_KERNEL);
	if (IS_ERR(data))
		return -ENOMEM;

	data->clk_io =
		devm_ioremap(&pdev->dev, pdev->resource->start,
			     pdev->resource->end - pdev->resource->start + 1);
	if (data->clk_io == NULL)
		return -EIO;

	if (of_find_property(np, "clock-output-names", NULL))
		of_property_read_string(np, "clock-output-names", &init.name);
	else
		init.name = "generic-clk";

	init.ops = &generic_clk_ops;
	init.parent_names = NULL;
	init.num_parents = 0;
	init.flags = 0;

	data->hw.init = &init;
	clk = devm_clk_register(&pdev->dev, &data->hw);
	if (IS_ERR(clk))
		return PTR_ERR(clk);

	return of_clk_add_provider(np, of_clk_src_simple_get, clk);
}

static const struct of_device_id generic_clk_ids[] = {
	{ .compatible = "generic-clk" },
	{ /* sentinel */ },
};

static struct platform_driver generic_clk_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "generic-clk",
		.of_match_table = generic_clk_ids,
	},
	.probe = generic_clk_probe,
};
builtin_platform_driver(generic_clk_driver);

MODULE_AUTHOR("Chowe");
MODULE_DESCRIPTION("generic clk driver for common");
MODULE_LICENSE("GPL v2");
