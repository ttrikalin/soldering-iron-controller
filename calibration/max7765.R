dat <- read.csv(file = "./max7765.csv")
dat$raw2 <- dat$raw^2
dat$raw3 <- dat$raw^3

mod0 <- lm(extech ~ raw , data = dat)
mod1 <- lm(extech ~ raw + raw2, data = dat)
mod2 <- lm(extech ~ raw + raw2 + raw3, data = dat)
lmtest::lrtest(mod0, mod1, mod2)

dat_pred <- data.frame(raw = seq(from = min(dat$raw), to = max(dat$raw), length.out = 100)) 
dat_pred$raw2 <- dat_pred$raw^2
dat_pred$raw3 <- dat_pred$raw^3

dat_pred$pred0 <- predict(mod0, newdata = dat_pred)
dat_pred$pred1 <- predict(mod1, newdata = dat_pred)
dat_pred$pred2 <- predict(mod2, newdata = dat_pred)

txt0 <- paste0("Linear: ", 
               format(mod0$coefficients[1], scientific = T, digits = 5), " + ", 
               format(mod0$coefficients[2], scientific = T, digits = 5), " * x")

txt1 <- paste0("Quadratic: ", 
               format(mod1$coefficients[1], scientific = T, digits = 5), " + ", 
               format(mod1$coefficients[2], scientific = T, digits = 5), " * x + ",
               format(mod1$coefficients[3], scientific = T, digits = 5), " * x^2")

txt2 <- paste0("Cubic: ", 
               format(mod2$coefficients[1], scientific = T, digits = 5), " + ", 
               format(mod2$coefficients[2], scientific = T, digits = 5), " * x + ",
               format(mod2$coefficients[3], scientific = T, digits = 5), " * x^2 + ",
               format(mod2$coefficients[4], scientific = T, digits = 5), " * x^3")


library(ggplot2)
colors <- c("blue", "red", "black")
names(colors) <- c(txt0, txt1, txt2)
ggplot(data = dat, aes(x = raw, y=extech)) + 
  geom_point() + 
  geom_line(data = dat_pred, aes(y = pred0, x = raw, color = txt0)) +
  geom_line(data = dat_pred, aes(y = pred1, x = raw, color = txt1)) +
  geom_line(data = dat_pred, aes(y = pred2, x = raw, color = txt2)) +
  labs( y = "EXTECH K thermocouple (Celsius)",
        x = "MAX6675 Celsius reading in Aoyue 906", 
        color =  "Legend") +
  scale_color_manual(values = colors) + 
  theme(legend.position =c( x = 0.35, y = 0.85)) #+ 
  #annotate("text", x = 0, y = 380, label = txt0, hjust = "left")

ggsave("calibration.pdf", width = 8, height = 6)
  